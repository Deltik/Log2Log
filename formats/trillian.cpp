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
    chart["GOOGLE"]   = "gtalk";
    chart["JABBER"]   = "jabber";
    chart["ICQ"]      = "icq";
    chart["IRC"]      = "irc";
    chart["MSN"]      = "msn";
    chart["SKYPE"]    = "skype";
    chart["YAHOO"]    = "yahoo";
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
    // Generated Log Container
    QVariant $log_generated;
    QMap<QString, QVariant>  $individuals;
    QHash<QString, QVariant> $individual;
    QList<QVariant>          $individual_time_assoc;
    // Counter
    int C = 1;
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

        // Trillian-ize the protocol
        QString protocol = $protocol;
        QMap<QString, QVariant>::const_iterator i = chart.constBegin();
        while (i != chart.constEnd())
        {
            if (i.value() == protocol)
            {
                protocol = i.key();
            }
            ++ i;
        }
        protocol = $protocol.toUpper();

        // START PATCH: Proper FACEBOOK naming
        if (protocol.toLower() == "facebook" && QVariant($with).toLongLong() > 0 && !$with.contains("chat.facebook.com"))
            $with = "-" + $with + "@chat.facebook.com";
        // END PATCH

        // START PATCH: Proper GOOGLE naming (not 100% accurate, but gets most people's personal GMail addresses)
        if (protocol.toLower() == "jabber" && ($account.endsWith("@gmail.com") ||
                                               $account.endsWith("@googlemail.com")))
            protocol = "GOOGLE";
        // END PATCH

        // Find associated individual, if existing
        $individual = $individuals[protocol + "/Query/" + $with + ".xml"].toHash();
        QString content = $individual["data"].toString();
        /// DISABLED \\\ $individual_time_assoc = $individual["time_assoc"].toList();

        QString $starter = "<session type=\"start\" time=\"" +
                           QVariant($time_base / 1000).toString() +
                           "\" ms=\"" +
                           QVariant($time_base % 1000).toString() +
                           "\" medium=\"" +
                           protocol +
                           "\" to=\"" +
                           $with +
                           "\" from=\"" +
                           $account +
                           "\"/>";

        /// DISABLED \\\ $individual = insertRow($content, $time_cur, $individual);
        autoLineBreak(&content);
        content += $starter;

        // Row tracker
        int r = 0;

        // Go through each chat row.
        while ($log->nextRow())
        {
            // Make array items more readily accessible.
            qint64  $time_cur    = $log->getTime();
            int     $code        = $log->getCode();
            QString $sender      = $log->getSender();
            QString $alias       = $log->getAlias();
            QString $message     = $log->getContent();
            int     $precision   = $log->getPrecision();
            int     $accuracy    = $log->getAccuracy();
            int     $nice        = $log->getNice();

            // Auto line break
            if (!content.endsWith("\n"))
                content += "\n";

            // If we're looking at a system message...
            if ($code >= 1)
            {
                // Conversation open (_evt_open)
                if ($sender.startsWith("_evt_open") && r != 0)
                {
                    QString $content;
                    $content = "<session type=\"start\" time=\"" +
                               QVariant($time_cur / 1000).toString() +
                               "\" ms=\"" +
                               QVariant($time_cur % 1000).toString() +
                               "\" medium=\"" +
                               protocol +
                               "\" to=\"" +
                               $with +
                               "\" from=\"" +
                               $account +
                               "\"/>";

                    /// DISABLED \\\ $individual = insertRow($content, $time_cur, $individual);
                    autoLineBreak(&content);
                    content += $content;
                }
                // Conversation close (_evt_close)
                if ($sender.startsWith("_evt_close"))
                {
                    QString $content;
                    $content = "<session type=\"stop\" time=\"" +
                               QVariant($time_cur / 1000).toString() +
                               "\" ms=\"" +
                               QVariant($time_cur % 1000).toString() +
                               "\" medium=\"" +
                               protocol +
                               "\" to=\"" +
                               QUrl::toPercentEncoding($with) +
                               "\" from=\"" +
                               QUrl::toPercentEncoding($account) +
                               "\"/>";

                    /// DISABLED \\\ $individual = insertRow($content, $time_cur, $individual);
                    autoLineBreak(&content);
                    content += $content;
                }
                // Close off
                else if (!$log->hasNextRow())
                {
                    QString $content;
                    $content = "<session type=\"stop\" time=\"" +
                               QVariant($time_cur / 1000).toString() +
                               "\" ms=\"" +
                               QVariant($time_cur % 1000).toString() +
                               "\" medium=\"" +
                               protocol +
                               "\" to=\"" +
                               QUrl::toPercentEncoding($with) +
                               "\" from=\"" +
                               QUrl::toPercentEncoding($account) +
                               "\"/>";

                    /// DISABLED \\\ $individual = insertRow($content, $time_cur, $individual);
                    autoLineBreak(&content);
                    content += $content;
                }
            }
            // Otherwise, it's a normal message...
            else
            {
                // Common Replacements
                if ($sender == "_with")
                    $sender = $with;
                if ($sender == "_self")
                    $sender = $account;
                if ($alias == "_with")
                    $alias = $with_alias;
                if ($alias == "_self")
                    $alias = $self_alias;

                // If message sender was _self
                if ($sender == $account)
                {
                    QString $content;
                    $content = "<message type=\"outgoing_privateMessage\" time=\"" +
                               QVariant($time_cur / 1000).toString() +
                               "\" ms=\"" +
                               QVariant($time_cur % 1000).toString() +
                               "\" medium=\"" +
                               protocol +
                               "\" to=\"" +
                               QUrl::toPercentEncoding($with) +
                               "\" from=\"" +
                               QUrl::toPercentEncoding($sender) +
                               "\" from_display=\"" +
                               QUrl::toPercentEncoding($alias) +
                               "\" text=\"" +
                               QUrl::toPercentEncoding($message) +
                               "\"/>";

                    /// DISABLED \\\ $individual = insertRow($content, $time_cur, $individual);
                    autoLineBreak(&content);
                    content += $content;
                }
                // Otherwise, message sender was _with or someone else
                else
                {
                    QString $content;
                    $content = "<message type=\"incoming_privateMessage\" time=\"" +
                               QVariant($time_cur / 1000).toString() +
                               "\" ms=\"" +
                               QVariant($time_cur % 1000).toString() +
                               "\" medium=\"" +
                               protocol +
                               "\" to=\"" +
                               QUrl::toPercentEncoding($account) +
                               "\" from=\"" +
                               QUrl::toPercentEncoding($sender) +
                               "\" from_display=\"" +
                               QUrl::toPercentEncoding($alias) +
                               "\" text=\"" +
                               QUrl::toPercentEncoding($message) +
                               "\"/>";

                    /// DISABLED \\\ $individual = insertRow($content, $time_cur, $individual);
                    autoLineBreak(&content);
                    content += $content;
                }

                // Close off
                if (!$log->hasNextRow())
                {
                    QString $content;
                    $content = "<session type=\"stop\" time=\"" +
                               QVariant($time_cur / 1000).toString() +
                               "\" ms=\"" +
                               QVariant($time_cur % 1000).toString() +
                               "\" medium=\"" +
                               protocol +
                               "\" to=\"" +
                               QUrl::toPercentEncoding($with) +
                               "\" from=\"" +
                               QUrl::toPercentEncoding($account) +
                               "\"/>";

                    /// DISABLED \\\ $individual = insertRow($content, $time_cur, $individual);
                    autoLineBreak(&content);
                    content += $content;
                }
            }

            r ++;
        }

        // Save the modified $individual
        /// DISABLED \\\ $individual["modtime"] = $individual["time_assoc"].toList().last().toLongLong() / 1000;
        $individual["data"] = content;
        $individuals[protocol + "/Query/" + $with + ".xml"] = $individual;

        // Increment the entry key.
        C++;
        // Update the progress bar.
        updateProgress((40 * C / total) + 50, "Converted " + QVariant(C).toString() + "/" + QVariant(total).toString() + " files...");
    }

    // Return
    $log_generated = $individuals;
    return $log_generated;
}

/**
 * TRILLIAN_CUSTOM: Find Insert Line
 */
int Trillian::findInsertLine(qint64 time, QVariantList assoc)
{
    for (int i = 0; i < assoc.size(); i ++)
    {
        if (time > assoc[i].toLongLong())
            return i;
    }
    return 0;
}

/**
 * TRILLIAN_CUSTOM: Do Insert Line
 */
QString Trillian::insertLine(int line, QString text, QString toInsert)
{
    QStringList split = text.split("\n");
    split.insert(line, toInsert);
    text = split.join("\n");

    return text;
}

/**
 * TRILLIAN_CUSTOM: Insert Entry Row
 */
QVariantHash Trillian::insertRow(QString text, qint64 time, QVariantHash individual)
{
    QVariantList individual_time_assoc = individual["time_assoc"].toList();
    int line = findInsertLine(time, individual_time_assoc);
    QString newText = insertLine(line, individual["data"].toString(), text);
    individual_time_assoc.insert(line, time);
    individual["data"] = newText;
    individual["time_assoc"] = individual_time_assoc;

    return individual;
}

/**
 * HELPER: Modulus for a Very Large Number
 */
int Trillian::modulus(qint64 dividend, int divisor)
{
    QString dividend_string = QVariant(dividend).toString();
    QString divisor_string  = QVariant(divisor) .toString();
    int     chunk_size      = divisor_string.length() + 1;
    int     position        = 0;
    int     modulus         = dividend;

    while (dividend_string.length() > 0)
    {
        QString part = dividend_string.mid(position, chunk_size);
        int part_int = part.toInt();
        int part_mod = part_int % divisor;

        dividend_string = dividend_string.mid(position);
        position += chunk_size;

        if (dividend_string.length() > 0)
            dividend_string.append(QVariant(part_mod).toString());
        else
            modulus = part_mod;
    }

    return modulus;
}

/**
 * TRILLIAN_CUSTOM: Automatically Append Line Break
 */
void Trillian::autoLineBreak(QString *text)
{
    if (!text->endsWith("\n") && text->length() > 0)
        text->append("\n");
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
