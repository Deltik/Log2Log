/**
 * Log2Log
 *  Formats
 *   Digsby
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

#include "digsby.h"
#include "helper.h"

Digsby::Digsby()
{
    final = new StdFormat();
    final->setClient("Digsby");
}

/**
 * Load a Chat Log
 */
void Digsby::load(QVariant $log_raw)
{
    // If $log_raw does not contain a Digsby chat log...
    if (!$log_raw.toString().startsWith("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"))
    {
        return;
    }

    // Import the chat log.
    QString $log_proc = $log_raw.toString();

    // CONSTRUCT: _entry
    final->newEntry();

    // Get _self and _with
    QString $self, $with;
    $self = accountGuess;
    // Non-group chat ( _with defined)
    if ($log_proc.contains("<TITLE>IM Logs with "))
    {
        QString $users_proc = $log_proc;
        QStringList $users_split = $users_proc.split("<TITLE>IM Logs with ");
        $users_split.pop_front();
        $users_proc = $users_split.join("<TITLE>IM Logs with ");
        $users_split = $users_proc.split(" on ");
        $with = $users_split.takeFirst();
        QString $timebase_proc = $users_split.join(" on ");
        QStringList $timebase_split = $timebase_proc.split("</TITLE>");
        QString $timebase = $timebase_split.takeFirst();
        QDateTime dt = QDateTime::fromString($timebase, "yyyy-MM-dd");
        qlonglong $datetime = dt.toMSecsSinceEpoch();
        //qDebug() << "HEREWEGO: " << $datetime;

        // CONSTRUCT: _time
        //final->setTime($datetime);
        // CONSTRUCT: _timezone
        //final->setTimezone("utc");
    }
    // Group chat (sorry; there's no _with information)
    else
    {
        $with = "Group Chat";
        QString $chatin_proc = $log_proc;
        QStringList $chatin_split = $chatin_proc.split("<TITLE>Chat in ");
        $chatin_split.pop_front();
        $chatin_proc = $chatin_split.join("<TITLE>Chat in ");
        $chatin_split = $chatin_proc.split(" on ");
        $chatin_split.takeFirst();
        QString $timebase_proc = $chatin_split.join(" on ");
        QStringList $timebase_split = $timebase_proc.split("</TITLE>");
        QString $timebase = $timebase_split.takeFirst();
        QDateTime dt = QDateTime::fromString($timebase, "yyyy-MM-dd");
        qlonglong $datetime = dt.toMSecsSinceEpoch();
        //qDebug() << "HEREWEGO2: " << $datetime;

        // CONSTRUCT: _time
        //final->setTime($datetime);
        // CONSTRUCT: _timezone
        //final->setTimezone("utc");
    }

    // Remove cruft that will interfere with QXmlStreamReader.
    QStringList $log_split = $log_proc.split("</HEAD>");
    $log_split.pop_front();
    $log_proc = $log_split.join("</HEAD>");
    $log_proc.replace("<BODY onload=\"utc_to_local()\">", "");
    $log_proc = "<meaningless>" + $log_proc + "</meaningless>";

    // Create HTML reader
    QXmlStreamReader xml($log_proc);
    //qDebug() << $log_proc;

    // CONSTRUCT: _self
    final->setSelf($self);
    // CONSTRUCT: _with
    final->setWith($with);
    // CONSTRUCT: _protocol
    final->setProtocol(protocol);

    // Read the HTML file.
    while (!xml.atEnd())
    {
        // Read next item
        QXmlStreamReader::TokenType token = xml.readNext();
        //qDebug() << xml.tokenString();

        // Just some items that might be used in this scope
        qlonglong               $datetime;
        QString                 $self_alias;
        QString                 $with_alias;
        QString                 $class;

        // Looking at element beginnings...
        if (token == QXmlStreamReader::StartElement)
        {
            // If token is the chat row
            if (xml.attributes().value("class").endsWith(" message"))
            {
                // What kind of chat row, again?
                $class = xml.attributes().value("class").toString();

                // New chat row
                final->newRow();

                // Reset upper scope variables
                $datetime   = NULL;
                $self_alias . clear();
                $with_alias . clear();

                // Write datetime
                QString $datetime_raw = xml.attributes().value("timestamp").toString();
                QDateTime dt = QDateTime::fromString($datetime_raw, "yyyy-MM-dd HH:mm:ss");
                $datetime = dt.toMSecsSinceEpoch();
                //qDebug() << $datetime;
                final->setTime($datetime);
                final->setTimezone("utc");

                // Point to the sender wrapper: <span class="buddy">
                xml.readNext();
                // Point to the sender alias
                xml.readNext();
                // Handle sender
                QString $sender = xml.text().toString().trimmed();

                // CONSTRUCT: _sender
                if ($class == "outgoing message")
                {
                    final->setSender("_self");
                    // CONSTRUCT: _self_alias
                    $self_alias = $sender;
                    final->setSelfAlias($self_alias);
                }
                else if ($class == "incoming message")
                {
                    final->setSender("_with");
                    // CONSTRUCT: _with_alias
                    $with_alias = $sender;
                    final->setWithAlias($with_alias);
                }
                else
                    final->setSender("_unknown");
                // CONSTRUCT: _sender_alias
                final->setAlias($sender);

                // Entering: <span class="msgcontent">
                xml.skipCurrentElement();
                xml.readNextStartElement();
                //qDebug() << xml.attributes().value("class");

                // Content variable
                qint64 start = xml.characterOffset();
                QString $message = xml.readElementText(QXmlStreamReader::IncludeChildElements);
                qint64 finish = xml.characterOffset();
                $message = $log_proc.mid(start, finish-start).replace(QRegularExpression("<\\/span>$"), "");
                // CONSTRUCT: _message
                final->setContent($message);
                xml.readNext();
                //qDebug() << xml.name().toString() << xml.isEndElement();
            }
        }
    }
    //qDebug() << "";
}

/**
 * Generate Log from Standardized Log
 */
QVariant Digsby::generate(StdFormat *$log)
{
    // TODO
}

/**
 * Process "From" Request
 */
StdFormat* Digsby::from(QHash<QString, QVariant> data)
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

        /// "%CHATLOGSPATH%/USERNAMEDIGSBY/PROTOCOL/USERNAMESELF/USERNAMEBUDDY_PROTOCOL/YYYY-MM-DD.html"
        QString unames_proc = i.key();

        // Guess the account and buddy (not accurate)
        accountGuess.clear();
        buddyGuess.clear();
        if (unames_proc.endsWith(".html"))
        {
            unames_proc = QDir::fromNativeSeparators(unames_proc);
            unames_proc.remove(unames_proc.length() - 5, 5);
            //qDebug() << unames_proc;
            QStringList unames_split = unames_proc.split("/");
            // Datestamp (unused)
            unames_split.takeLast();
            // Buddy Username
            buddyGuess = unames_split.takeLast();
            QStringList uname_split = buddyGuess.split("_");
            protocol = uname_split.takeLast();
            buddyGuess = uname_split.join("_");
            // Group Mode Toggle
            if (buddyGuess == "Group Chats")
                group = true;
            else
                group = false;

            // Account Username
            accountGuess = unames_split.takeLast();
            accountGuess.remove(accountGuess.length() - 10);

            // Protocol
            protocol = unames_split.takeLast();

            this->load($raw_item);
        }

        c++;
        updateProgress((40 * c / list.count()) + 10, "Interpreted " + QVariant(c).toString() + "/" + QVariant(list.count()).toString() + " files...");
        i ++;
    }
    updateProgress(50, "Filling in data gaps...");
    Helper::postprocessor(final);

    // Step 3/3: Submit the Log2Log-standardized chat log array.
    emit finished();
    return this->final;
}

/**
 * Process "To" Request
 */
void Digsby::to(StdFormat* $log)
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
