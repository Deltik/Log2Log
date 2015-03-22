/**
 * Log2Log
 *  Formats
 *   Windows Live Messenger
 *
 * @author Deltik
 *
 * License:
 *  This file is part of Log2Log.
 *
 *  Log2Log is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Log2Log is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Log2Log.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "wlm.h"
#include "helper.h"

Wlm::Wlm()
{
    final = new StdFormat();
    final->setClient("Windows Live Messenger");
}

/**
 * Load a Chat Log
 */
void Wlm::load(QVariant $log_raw)
{
    // If $log_raw does not contain a WLM chat log...
    if (!$log_raw.toString().startsWith("<?xml version=\"1.0\"?>\n<?xml-stylesheet type='text/xsl' href='MessageLog.xsl'?>"))
    {
        return;
    }

    // $log_raw is WLM chat log. Create new entry.
    final->newEntry();
    final->setSelf(accountGuess);
    final->setSelfAlias("Unknown");
    final->setWith(buddyGuess);
    final->setWithAlias("Unknown");

    // Create XML reader
    QXmlStreamReader xml($log_raw.toString());

    // Entering: <?xml version="1.0"?>
    xml.readNext();
    // Entering: <?xml-stylesheet type='text/xsl' href='MessageLog.xsl'?>
    xml.readNext();
    // Entering: <Log FirstSessionID="_INT" LastSessionID="_INT">
    xml.readNext();

    int begin = xml.attributes().value("FirstSessionID").toString().toInt();
    int end   = xml.attributes().value("LastSessionID").toString().toInt();
    int curid = begin - 1;

    // Variables that are needed in the while() scope below:
    QString userMode = "unknown";

    // Read the XML file
    while (!xml.atEnd())
    {
        // Read next item
        QXmlStreamReader::TokenType token = xml.readNext();

        // Looking at element beginnings...
        if (token == QXmlStreamReader::StartElement)
        {
            // If entering DateTime container: <TAG DateTime="DATETIME">
            if (xml.attributes().hasAttribute("DateTime"))
            {
                // Extract entry ID
                int compare = xml.attributes().value("SessionID").toString().toInt();

                // Extract that ugly Microsoft DateTime that isn't exactly ISODate
                QString datetime_cur = xml.attributes().value("DateTime").toString();
                QDateTime datetime_proc = QDateTime::fromString(datetime_cur, Qt::ISODate);

                // If new entry
                if (compare != curid)
                {
                    final->newEntry();
                    curid = compare;
                    final->setTime(datetime_proc.toMSecsSinceEpoch());
                }

                // Move to chat rows
                final->newRow();
                // Set time
                final->setTime(datetime_proc.toMSecsSinceEpoch());
                final->setPrecision(-1);

                // Determine chat row type
                if (xml.qualifiedName().toString() == "Message")
                {
                    final->setCode(0);
                    final->setSender("_unknown");
                }
                if (xml.qualifiedName().toString() == "Invitation")
                {
                    final->setCode(1);
                    final->setSender("_evt");
                }
                if (xml.qualifiedName().toString() == "InvitationResponse")
                {
                    final->setCode(1);
                    final->setSender("_evt");
                }
            }

            // If entering `From` identifier: <From>
            if (xml.qualifiedName().toString() == "From")
                userMode = "from";

            // If entering `To` identifier: <To>
            if (xml.qualifiedName().toString() == "To")
                userMode = "to";

            // If entering sender or receiver: <User>
            if (xml.qualifiedName().toString() == "User")
            {
                if (userMode == "from")
                {

                    final->setAlias(xml.attributes().value("FriendlyName").toString());
                }
            }

            // If entering message content: <Text>
            if (xml.qualifiedName().toString() == "Text")
            {
                QString css = xml.attributes().value("Style").toString().trimmed();
                if (!css.isEmpty())
                    css = "<font style=\"" + css + "</font>";
                // Entering: CONTENT
                xml.readNext();
                final->setContent(xml.text().toString());
            }
        }
    }
}

/**
 * Generate Log from Standardized Log
 */
QVariant Wlm::generate(StdFormat *$log)
{
    updateProgress(50, "Conversion starting...");
    // Generated Log Container
    QVariant $log_generated;
    QMap<QString, QVariant> $log_new;
    QHash<QString, QVariant> $info;
    // Counter
    int $i = 1;
    // Browser
    $log->resetPointer();

    while ($log->nextEntry())
    {
        // Put the longer variables into something more readily accessible.
        QString $protocol      = $log->getProtocol();
        QString $account       = $log->getSelf();
        QString $self_alias    = $log->getSelfAlias();
        QString $with          = $log->getWith();
        QString $with_alias    = $log->getWithAlias();
        qlonglong $time_base   = $log->getTime();
        QString $timezone_base = $log->getTimezone();

        // Determine file to write to
        QString $relname       = mkWlmId($account) + "/" +
                "History" +
                "/" +
                mkWlmId($with) +
                ".xml";

        QString $content;
        if (wlm_track.contains($relname))
        {
            wlm_track[$relname]++;
            // Chat log header
            $info = $log_new[$relname].toHash();
            $content = $info["content"].toString().replace(QRegExp("</Log>$"), "");
        }
        else
        {
            wlm_track[$relname] = 1;
            // Chat log header
            $content = "<?xml version=\"1.0\"?>\n<?xml-stylesheet type='text/xsl' href='MessageLog.xsl'?>\n<Log>";
        }

        // Go through each chat row.
        while ($log->nextRow())
        {
            // Make array items more readily accessible.
            qlonglong $time_cur  = $log->getTime();
            int     $code        = $log->getCode();
            QString $sender      = $log->getSender();
            QString $alias       = $log->getAlias();
            QString $message     = $log->getContent();
            int     $precision   = $log->getPrecision();
            int     $accuracy    = $log->getAccuracy();
            int     $nice        = $log->getNice();

            QLocale la; QString tmp = la.dateTimeFormat(QLocale::ShortFormat);
            QString $autoAP;
            if (tmp.toLower().contains("ap"))
                $autoAP = " AP";

            // 2015-03-22 :: Patch for blank "From" sender fields
            if ($alias.isEmpty() && !$sender.isEmpty())
            {
                $alias = $sender;
            }

            QString $recipient_alias = "";
            if ($sender == "_self" ||
                $sender == $account ||
                $sender == $self_alias)
            {
                if (!$with_alias.isEmpty())
                    $recipient_alias = $with_alias;
                else
                    $recipient_alias = $with;
            }
            else if ($sender == "_with" ||
                     $sender == $with ||
                     $sender == $with_alias)
            {
                if (!$self_alias.isEmpty())
                    $recipient_alias = $self_alias;
                else
                    $recipient_alias = $account;
            }

            // <Message Date="3/13/2015" Time="5:09:36 PM" DateTime="2015-03-14T00:09:36.447Z" SessionID="1"><From><User FriendlyName="Tiana 禎安成"/></From><To><User FriendlyName="Mir (Adam)"/></To><Text Style="font-family:Lucida Calligraphy; color:#800080; ">test test</Text></Message>

            $content += "<Message Date=\"" +
                       QDateTime::fromMSecsSinceEpoch($time_cur).toString("M/d/yyyy") +
                       "\" Time=\"" +
                       QDateTime::fromMSecsSinceEpoch($time_cur).toString("h:mm:ss"+$autoAP) +
                       "\" DateTime=\"" +
                       QDateTime::fromMSecsSinceEpoch($time_cur).toString("yyyy-MM-ddTHH:mm:ss.zzzZ") +
                       "\" SessionID=\"" +
                       QVariant(wlm_track[$relname]).toString() +
                       "\"><From><User FriendlyName=\"" +
                       $alias.toHtmlEscaped() +
                       "\"/></From><To><User FriendlyName=\"" +
                       $recipient_alias.toHtmlEscaped() +
                       "\"/></To><Text Style=\"font-family:Lucida Calligraphy; color:#800080; \">" +
                       QTextDocument($message).toPlainText().toHtmlEscaped().replace("\n", "&#xD;") +
                       "</Text></Message>";
        }
        $content += "</Log>";
        $info["content"] = $content;
        $info["modtime"] = $time_base / 1000;
        $log_new[$relname] = $info;

        QString xslsrc = ":/resources/MessageLog.xsl";
        QFile* file = new QFile(xslsrc);
        file->open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream in(file);
        $log_new[mkWlmId($account) + "/History/MessageLog.xsl"] = in.readAll();

        // Increment the entry key.
        $i ++;
        // Update the progress bar.
        updateProgress((40 * $i / total) + 50, "Converted " + QVariant($i).toString() + "/" + QVariant(total).toString() + " files...");
    }

    $log_generated = $log_new;
    return $log_generated;
}

/**
 * Generate a Microsoft Passport Number from Windows Live ID
 * @brief Wlm::mkPassportNumber
 * @param input
 * @return QString Microsoft Passport Number
 */
QString Wlm::mkPassportNumber(QString input)
{
/// Ported from decompiled .NET code
///  Source: http://computerforensics.parsonage.co.uk/downloads/MSNFoldersSetup.zip
///  Decompiler: http://www.jetbrains.com/decompiler/
/*
    private void ButtonConvertEmail_Click(object sender, EventArgs e)
    {
      string str1 = Strings.LCase(this.TextBoxEmailAddress.Text);
      double num1 = 0.0;
      string str2 = Strings.LCase(str1);
      int num2 = 1;
      int num3 = Strings.Len(str2);
      int Start = num2;
      while (Start <= num3)
      {
        double num4 = num1 * 101.0 + (double) Strings.Asc(Strings.Mid(str2, Start, 1));
        num1 = num4 - Conversion.Fix(num4 / 4294967296.0) * 4294967296.0;
        checked { ++Start; }
      }
      this.TextBox1.Text = Conversions.ToString(num1);
    }
*/

    QString str1 = input.toLower();
    double num1 = 0.0;
    QString str2 = str1.toLower();
    int num2 = 0;
    int num3 = str2.length();
    int Start = num2;
    while (Start < num3)
    {
        double num4 = num1 * 101.0 + (double) str2.at(Start).toLatin1();
        num1 = num4 - (int)(num4 / 4294967296.0) * 4294967296.0;
        ++Start;
    }
    return QVariant(num1).toString();
}

/**
 * Generate a Windows Live Messenger User ID
 * @brief Wlm::mkWlmId
 * @param input
 * @return QString Unique Windows Live Messenger ID
 */
QString Wlm::mkWlmId(QString input)
{
    return input.split("@").at(0) + mkPassportNumber(input);
}

/**
 * Process "From" Request
 */
StdFormat* Wlm::from(QHash<QString, QVariant> data)
{
    // Step 1/3: Fetch the data.
    QMap<QString, QVariant> list;
    if (data["files"].isNull())
        list = Helper::files_get_contents(data["path"].toString());
    else
        list = data["files"].toMap();
    QMap<QString, QVariant>::const_iterator i = list.constBegin();

    // Step 2/3: Process the data.
    int c = 0;
    while (i != list.constEnd())
    {
        QVariant $raw_item = (i.value());

        /// "%CHATLOGSPATH%/My Received Files/your_username0123456789/History/buddy_username0123456789.xml"
        QString unames_proc = i.key();

        // Guess the account and buddy (not accurate)
        accountGuess.clear();
        buddyGuess.clear();
        if (unames_proc.endsWith(".xml"))
        {
            unames_proc = QDir::fromNativeSeparators(unames_proc);
            unames_proc.remove(unames_proc.length() - 4);
            QStringList unames_split = unames_proc.split("/");
            // Buddy Username
            buddyGuess = unames_split.takeLast();
            buddyGuess.remove(buddyGuess.length() - 10);
            // Cruft
            unames_split.pop_back();
            // Account Username
            accountGuess = unames_split.takeLast();
            accountGuess.remove(accountGuess.length() - 10);

            this->load($raw_item);
        }

        c++;
        updateProgress((40 * c / list.count()) + 10, "Interpreted " + QVariant(c).toString() + "/" + QVariant(list.count()).toString() + " files...");
        i ++;
    }

    // Step 3/3: Submit the Log2Log-standardized chat log array.
    emit finished();
    return this->final;
}

/**
 * Process "To" Request
 */
void Wlm::to(StdFormat* $log)
{
    // Count Log Entries
    updateProgress(50, "Counting Log Entries...");
    $log->resetPointer();
    total = 0;
    while ($log->nextEntry())
        {
            total ++;
            updateProgress(50, "Counting Log Entries... ("+QVariant(total).toString()+" found so far)");
        }

    data = this->generate($log);
    emit finished();
}
