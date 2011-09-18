/**
 * Log2Log
 *  Formats
 *   Trillian XML
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

#include "trillian.h"
#include "helper.h"

Trillian::Trillian()
{
    final = new StdFormat();
    chart["AIM"]      = "aim";
    chart["ASTRA"]    = "astra";
    chart["BONJOUR"]  = "bonjour";
    chart["FACEBOOK"] = "facebook";
    chart["GOOGLE"]   = "jabber";
    chart["JABBER"]   = "jabber";
    chart["ICQ"]      = "icq";
    chart["IRC"]      = "irc";
    chart["MSN"]      = "msn";
    chart["SKYPE"]    = "skype";
    chart["YAHOO"]    = "yim";
}

/**
 * Load a Chat Log
 */
void Trillian::load(QVariant $log_raw)
{
    // QXmlStreamReader is a total conformist and will freak out if XML code is
    // not exactly the way it wants. For example, QXmlStreamReader will fail to
    // recognize <tag attr="something"/>, which is a format used throughout
    // Trillian chat logs. The following code turns the example into something
    // that will be accepted: <meaningless attr="something"></meaningless> .
    QString $log_proc = "<meaninglesses>\n" + $log_raw.toString() + "\n</meaninglesses>";
    $log_proc.replace("<session", "<meaningless");
    $log_proc.replace("<message", "<meaningless");
    $log_proc.replace("/>", "></meaningless>");
    $log_raw = $log_proc;

    // Create XML reader
    QXmlStreamReader test($log_raw.toString());
    QXmlStreamReader xml ($log_raw.toString());

    // Test the water
    test.readNextStartElement();
    test.readNextStartElement();
    // If file is not a Trillian XML chat log...
    if (!test.attributes().hasAttribute("medium"))
        return;

    // Safety: Don't try to edit entries if none exist yet.
    bool entryMade, selfSet, withSet = false;

    // Read the XML file
    while (!xml.atEnd())
    {
        // Read next item
        xml.readNextStartElement();

        QXmlStreamAttributes attr = xml.attributes();

        // If conversation start (_evt_open)
        if (attr.value("type") == "start")
        {
            final->newEntry(); entryMade = true;

            QString protocol = chart[attr.value("medium").toString()].toString();
            if (protocol.isEmpty())
                protocol = attr.value("medium").toString().toLower();
            final->setProtocol(protocol);

            qint64 time = attr.value("time").toString().toLongLong() * 1000 +
                          attr.value("ms").toString().toLongLong();
            final->setTime(time);

            final->newRow();
            final->setTime(time);
            final->setPrecision(-1);
            final->setCode(1);
            final->setSender("_evt_open");
            final->setContent(attr.value("from").toString());

            // Inaccurately guess _self and _with (should be corrected later)
            final->setSelf(QUrl::fromPercentEncoding(attr.value("from").toString().toAscii()));
            final->setWith(QUrl::fromPercentEncoding(attr.value("to").toString().toAscii()));
        }
        // If conversation close (_evt_close)
        else if (attr.value("type") == "stop")
        {
            qint64 time = attr.value("time").toString().toLongLong() * 1000 +
                          attr.value("ms").toString().toLongLong();

            if (!entryMade) { final->newEntry(); entryMade = true; }

            final->newRow();
            final->setTime(time);
            final->setPrecision(-1);
            final->setCode(1);
            final->setSender("_evt_close");
            final->setContent(attr.value("from").toString());
        }

        // If element was sent by _self (_msg_self)
        if (attr.value("type") == "outgoing_privateMessage")
        {
            if (!selfSet)
            {
                final->setSelf(QUrl::fromPercentEncoding(attr.value("from").toString().toAscii()));
                final->setSelfAlias(QUrl::fromPercentEncoding(attr.value("from_display").toString().toAscii()));
                selfSet = true;
            }
            if (!withSet)
            {
                final->setWith(QUrl::fromPercentEncoding(attr.value("to").toString().toAscii()));
                final->setWithAlias(QUrl::fromPercentEncoding(attr.value("to_display").toString().toAscii()));
                selfSet = true;
            }

            qint64 time = attr.value("time").toString().toLongLong() * 1000 +
                          attr.value("ms").toString().toLongLong();

            final->newRow();
            final->setCode(0);
            final->setTime(time);
            final->setPrecision(-1);
            final->setSender(QUrl::fromPercentEncoding(attr.value("from").toString().toAscii()));
            final->setAlias(QUrl::fromPercentEncoding(attr.value("from_display").toString().toAscii()));
            final->setContent(QUrl::fromPercentEncoding(attr.value("text").toString().toAscii()));
        }
        // If element was sent by _with (_msg_with)
        if (attr.value("type") == "incoming_privateMessage")
        {
            if (!selfSet)
            {
                final->setSelf(QUrl::fromPercentEncoding(attr.value("to").toString().toAscii()));
                final->setSelfAlias(QUrl::fromPercentEncoding(attr.value("to_display").toString().toAscii()));
                selfSet = true;
            }
            if (!withSet)
            {
                final->setWith(QUrl::fromPercentEncoding(attr.value("from").toString().toAscii()));
                final->setWithAlias(QUrl::fromPercentEncoding(attr.value("from_display").toString().toAscii()));
                selfSet = true;
            }

            qint64 time = attr.value("time").toString().toLongLong() * 1000 +
                          attr.value("ms").toString().toLongLong();

            final->newRow();
            final->setCode(0);
            final->setTime(time);
            final->setPrecision(-1);
            final->setSender(QUrl::fromPercentEncoding(attr.value("from").toString().toAscii()));
            final->setAlias(QUrl::fromPercentEncoding(attr.value("from_display").toString().toAscii()));
            final->setContent(QUrl::fromPercentEncoding(attr.value("text").toString().toAscii()));
        }
    }
}

/**
 * Generate Log from Standardized Log
 */
QVariant Trillian::generate(StdFormat *$log)
{
    // TODO
}

/**
 * Process "From" Request
 */
StdFormat* Trillian::from(QHash<QString, QVariant> data)
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
void Trillian::to(StdFormat* $log)
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
