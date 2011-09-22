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
    final->setSelf("_unknown_1");
    final->setSelfAlias("Unknown");
    final->setWith("_unknown");
    final->setWithAlias("Unknown");

    // QXmlStreamReader is a total conformist and will freak out if XML code is
    // not exactly the way it wants. For example, QXmlStreamReader will fail to
    // recognize <User attr="something"/>, which is a format used throughout
    // WLM chat logs. The following code turns the example into something
    // that will be accepted: <User attr="something"></User> .
    QString $log_proc = $log_raw.toString();
    $log_proc.replace("/>", "</User>");
    $log_raw = $log_proc;

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
    // TODO
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
        this->load($raw_item);
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
