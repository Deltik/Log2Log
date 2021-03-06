/**
 * Log2Log
 *  Formats
 *   Pidgin
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

/**
 * TODO: THIS FORMAT CONVERTER IS INCOMPLETE. List of incomplete features:
 *  - Time Precision
 *  - Timezones
 *  - Group Chat
 *  - File Transfers
 *  - /me
 *  - System Log
 *  - More that have yet to be discovered...
 */

#include "pidgin.h"
#include "helper.h"
#include <QtXml>
#include <QDateTime>

/**
 * Constructor
 */
Pidgin::Pidgin()
{
    final = new StdFormat();
    final->setClient("Pidgin");
}

/**
 * Load a Chat Log
 */
void Pidgin::load(QVariant $log_raw)
{
    // There are two types of Pidgin chat logs:
    //  - HTML
    //  - Plain text
    // Which is it gonna be?
    QString log_raw = $log_raw.toString();
    if (log_raw.left(6) == "<html>" || log_raw.startsWith("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\"><html>"))
        loadHtml($log_raw);
    else
        loadPlainText($log_raw);
}

/**
 * Load an HTML Pidgin Chat Log
 */
void Pidgin::loadHtml(QVariant $log_raw)
{
    // Import the chat log.
    QString $log_proc = $log_raw.toString();

    // Assume that the inputted log is in Pidgin HTML. Create new entry.
    final->newEntry();

    // Remove cruft that will interfere with QXmlStreamReader.
    QStringList $log_split = $log_proc.split("</head>");
    $log_split.pop_front();
    $log_proc = $log_split.join("</head>").trimmed();
    if ($log_proc.endsWith("</html>"))
    {
        $log_proc.chop(sizeof("</html>"));
    }

    // Create the HTML reader.
    QXmlStreamReader xml($log_proc);

    // 2015-03-20 :: Patch to help QXmlStreamReader go line by line
    int line_cur = 0;
    int line_prev = 0;
    bool last_iteration_was_chat_row = false;

    // Read the HTML file.
    while (!xml.atEnd())
    {
        // Read next item
        QXmlStreamReader::TokenType token = xml.readNext();

        // 2015-03-20 :: Patch to help QXmlStreamReader go line by line
        line_cur = xml.lineNumber();
        while (last_iteration_was_chat_row && line_cur == line_prev)
        {
            xml.readNext();
            line_cur = xml.lineNumber();
            if (xml.atEnd())
                break;
        }
        line_prev = line_cur;
        last_iteration_was_chat_row = false;

        // Just some items that might be used in this scope
        qlonglong               $time_base;
        QMap<QString, QVariant> $equizone;
        QString                 $with;
        QString                 $with_alias;
        QString                 $self;
        QString                 $self_alias;
        QString                 $file_transfer_by;

        // Looking at element beginnings...
        if (token == QXmlStreamReader::StartElement)
        {
            QString qualifiedName = xml.qualifiedName().toString();
            // If token is the log meta indicator...
            if (qualifiedName.contains(QRegExp("^h[1-6]$")) == true)
            {
                // Point to the data.
                xml.readNext();

                // Here, we look at Pidgin's chat log or system log header:
                //  - "Conversation with _WITH at _DAY_OFWEEK_3LETTERS _DAY_2DIGITS _MONTH_3LETTERS _YEAR_4DIGITS _TIME_BASE _TIMEZONE_ABBREVIATION on _SELF (_PROTOCOL)"
                //  - "Conversation with _WITH at 4/13/2021 12:24:19 PM on _SELF (_PROTOCOL)"
                //  - "System log for account _SELF (_PROTOCOL) connected at _DAY_OFWEEK_3LETTERS _DAY_2DIGITS _MONTH_3LETTERS _YEAR_4DIGITS _TIME_BASE _TIMEZONE_ABBREVIATION"
                QString $log_header = xml.text().toString();
                // If this is a system log, forward to the proper method.
                if ($log_header.left(6) == "System")
                    return loadSystemHtml($log_raw);

                QRegExp headerMatcher("Conversation with (.*) at (.*) on (.*) \\(([^\\)]+)\\)");
                headerMatcher.indexIn($log_header);
                final->setWith(headerMatcher.cap(1));
                QString $date_proc = headerMatcher.cap(2);
                final->setSelf(headerMatcher.cap(3));
                final->setProtocol(headerMatcher.cap(4));

                QDateTime test = QLocale::system().toDateTime($date_proc, QLocale::ShortFormat);
                QStringList $date_split = $date_proc.split(" ");
                QString $timezone = $date_split.takeLast();
                if ($timezone.toLower().contains(QRegExp("^[ap]m$")))
                    $date_split.append($timezone);
                $date_proc = $date_split.join(" ");
                QDateTime $time_proc;
                if ($date_proc.contains(QRegExp(" [AP]M$")))
                    $time_proc = QDateTime::fromString($date_proc, "ddd dd MMM yyyy hh:mm:ss AP");
                else
                    $time_proc = QDateTime::fromString($date_proc, "ddd dd MMM yyyy hh:mm:ss");
                if (!$time_proc.isValid())
                {
                    qlonglong $datetime = interpretTime($date_proc);
                    $time_proc = QDateTime::fromMSecsSinceEpoch($datetime);
                }
                // CONSTRUCT: _time
                $time_base = $time_proc.toMSecsSinceEpoch();
                final->setTime($time_base);

                $equizone = Helper::zone_search($timezone);
                // CONSTRUCT: _timezone
                if ($equizone.empty())
                {
                    $timezone = QTimeZone::systemTimeZone().abbreviation($time_proc);
                    $equizone = Helper::zone_search($timezone);
                }
                final->setTimezone($equizone["full_tz_name"].toString());
            }

            // If token is a chat row
            else if (qualifiedName == "font" || qualifiedName == "span")
            {
                // 2015-03-20 :: Patch to help QXmlStreamReader go line by line
                last_iteration_was_chat_row = true;

                // Create new chat row
                final->newRow();
                // CONSTRUCT: _timezone
                final->setTimezone($equizone["full_tz_name"].toString());

                // Color Legend:
                //  #16569E : _self
                //  #A82F2F : _with
                //  #FF0000 : _evt_error
                //  #6C2585 : Whisper (not supported yet)
                //  #062585 : /me
                QXmlStreamAttributes attributes = xml.attributes();
                QString $sender_color;
                if (attributes.hasAttribute("color"))
                    $sender_color = attributes.value("color").toString();
                else if (attributes.hasAttribute("style"))
                {
                    QRegExp match("(#[0-9a-fA-F]{6})");
                    match.indexIn(attributes.value("style").toString());
                    $sender_color = match.cap(1);
                }

                // If element is an unhandled message type (_evt)
                if ($sender_color.isEmpty())
                {
                    // Entering text for timestamp
                    xml.readNext();
                    $time_base = interpretTime(xml.text().toString(), $time_base);
                    // CONSTRUCT: _time
                    final->setTime($time_base);
                    // CONSTRUCT: _time_precision
                    final->setPrecision(0);
                    // CONSTRUCT: _code
                    final->setCode(1);
                    // CONSTRUCT: _sender
                    final->setSender("_evt");
                }
                // If element is an error (_error)
                // (Log2Log Generic Chat Log Format 1.2 has a primitive error
                //  system. Just an _error should suffice for everyone for the
                //  time being.)
                else if ($sender_color == "#FF0000")
                {
                    // Entering element for timestamp
                    xml.readNext();
                    // Entering text for timestamp
                    xml.readNext();
                    $time_base = interpretTime(xml.text().toString(), $time_base);
                    // CONSTRUCT: _time
                    final->setTime($time_base);
                    // CONSTRUCT: _time_precision
                    final->setPrecision(0);
                    // CONSTRUCT: _code
                    final->setCode(1);
                    // CONSTRUCT: _sender
                    final->setSender("_error");
                    // Entering element for bold
                    xml.readNext();
                    // Entering error message
                    xml.readNext();
                    // CONSTRUCT: _message
                    final->setContent(xml.text().toString());
                }
                // If element was sent by _self (_msg_self)
                else if ($sender_color == "#16569E")
                {
                    // Entering element for timestamp
                    xml.readNext();
                    // Entering text for timestamp
                    xml.readNext();
                    $time_base = interpretTime(xml.text().toString(), $time_base);
                    // CONSTRUCT: _time
                    final->setTime($time_base);
                    // CONSTRUCT: _time_precision
                    final->setPrecision(0);
                    // CONSTRUCT: _sender
                    final->setSender("_self");
                }
                // If element was sent by _with (_msg_with)
                else if ($sender_color == "#A82F2F")
                {
                    // Entering element for timestamp
                    xml.readNext();
                    // Entering text for timestamp
                    xml.readNext();
                    $time_base = interpretTime(xml.text().toString(), $time_base);
                    // CONSTRUCT: _time
                    final->setTime($time_base);
                    // CONSTRUCT: _time_precision
                    final->setPrecision(0);
                    // CONSTRUCT: _sender
                    final->setSender("_with");
                }

                // Sadly, this QXmlStreamReader has extreme difficulty
                // processing the rest of the chat row. We must now rely on
                // good ol' explode()ions and implode()ions...
                QString $line = readLine($log_proc, xml.lineNumber());
                QStringList $line_split;
                bool oldPidgin = false;
                if ($line.startsWith("<font"))
                {
                    oldPidgin = true;
                    $line_split = $line.split(")</font>");
                }
                else
                {
                    $line_split = $line.split(")</span>");
                }
                $line_split.pop_front();
                if (oldPidgin)
                {
                    $line = $line_split.join(")</font>").trimmed();
                }
                else
                {
                    $line = $line_split.join(")</span>").trimmed();
                }

                // If there is a _sender identifier
                if ($line.left(3) == "<b>")
                {
                    $line_split = $line.split("<b>");
                    $line_split.pop_front();
                    $line = $line_split.join("<b>");
                    $line_split = $line.split("</b>");
                    QString $sender_id = $line_split.takeFirst();
                    $line = $line_split.join("</b>");
                    // Process Log2Log-supported events. TODO
                    QHash<QString, QString> $sweep_terms;
                    $sweep_terms[" has signed off."]            = "_evt_offline";
                    $sweep_terms[" has signed on."]             = "_evt_online";
                    $sweep_terms[" has gone away."]             = "_evt_away";
                    $sweep_terms[" is no longer away."]         = "_evt_available";
                    $sweep_terms[" has nudged you!"]            = "_evt_attention";
                    $sweep_terms[" has left the conversation."] = "_evt_close";
                    $sweep_terms[" wants to send you a file"]   = "_file_init";
                    QHashIterator<QString, QString> i($sweep_terms);
                    while (i.hasNext())
                    {
                        i.next();
                        QString $sender_alias = $sender_id.replace(i.key(), "");
                        if ($sender_alias != $sender_id)
                        {
                            if ($sender_alias == $with_alias)
                                // CONSTRUCT: _message
                                final->setContent($with);
                            else if ($sender_alias == $self_alias)
                                // CONSTRUCT: _message
                                final->setContent($self);
                            // else There is no _sender.
                            // CONSTRUCT: _code
                            final->setCode(1);
                            // CONSTRUCT: _sender
                            final->setSender(i.value());
                            // CONSTRUCT: _sender_alias
                            final->setAlias($sender_alias);

                            // Exception: File Transfer
                            $file_transfer_by = $sender_alias;

                            break;
                        }
                        // Exception: "Transfer of file ... complete"
                        if ($sender_id.contains("Transfer of file "))
                        {
                            QString $filename_proc;
                            QString $filename;
                            QString $src;
                            $filename_proc = $sender_id.mid(17).right(9);
                            QXmlStreamReader $filename_reader($filename_proc);
                            // Entering <a>
                            $filename_reader.readNext();
                            $src = $filename_reader.attributes().value("href").toString();
                            // Entering <a> innerHTML
                            $filename_reader.readNext();
                            $filename = $filename_reader.text().toString();
                            // CONSTRUCT: _code
                            final->setCode(1);
                            // CONSTRUCT: _sender
                            final->setSender("_file_done \"" +
                                             $file_transfer_by +
                                             "\" \"" +
                                             $filename +
                                             "\" \"" +
                                             $src +
                                             "\"");
                        }
                    }
                    if ($sender_id.right(1) == ":")
                    {
                        $sender_id.chop(1);
                        // CONSTRUCT: _sender
                        final->setAlias($sender_id);
                    }
                }

                // Finalize the message
                if (final->getCode() != 1 && final->getContent().isEmpty())
                {
                    // Cleanup
                    if ($line.endsWith("<br/>"))
                        $line.chop(5);
                    if ($line.endsWith("<br>"))
                        $line.chop(4);
                    $line = $line.trimmed();
                    if ($line.startsWith("</font>") || $line.startsWith("</span>"))
                        $line = $line.mid(7).trimmed();
                    $line = $line.replace("&quot;", "\"");
                    $line = $line.replace("&amp;", "&");
                    // CONSTRUCT: _message
                    final->setContent($line);
                }
            }
        }
    }
}

/**
 * Load a Plain Text Pidgin Chat Log
 */
void Pidgin::loadPlainText(QVariant $log_raw)
{
    // Import the chat log.
    QString $log_proc = $log_raw.toString();

    // Assume that the inputted log is in Pidgin TXT. Create new entry.
    final->newEntry();

    // 2015-03-22 :: Hopefully Deltik is a little smarter now when reading chat logs…
    qlonglong               $time_base;
    qlonglong               initial_time;
    QMap<QString, QVariant> $equizone;
    QString                 $sender_alias;
    QTextStream log_stream(&$log_proc);
    int linenum = 0;
    QRegExp msgstart_match("^[\\(\\[]\\d{1,2}:\\d{2}[:]{0,1}\\d{0,2}([ ]{0,1}[AP]M){0,2}[\\)\\]] ",
                           Qt::CaseInsensitive,
                           QRegExp::RegExp);
    QString msg_buffer = "";
    while (!log_stream.atEnd())
    {
        QString line = log_stream.readLine();
        linenum ++;

        if (linenum == 1)
        {
            // Unfortunately, RegExp can't help with this tricky next part.
            // Here, we look at Pidgin's chat log or system log header:
            //  - "Conversation with _WITH at _DAY_OFWEEK_3LETTERS _DAY_2DIGITS _MONTH_3LETTERS _YEAR_4DIGITS _TIME_BASE _TIMEZONE_ABBREVIATION on _SELF (_PROTOCOL)"
            //  - "System log for account _SELF (_PROTOCOL) connected at _DAY_OFWEEK_3LETTERS _DAY_2DIGITS _MONTH_3LETTERS _YEAR_4DIGITS _TIME_BASE _TIMEZONE_ABBREVIATION"
            QString $log_header = line;
            // If this is a system log, forward to the proper method.
            if ($log_header.left(6) == "System")
                return loadSystemPlainText($log_raw);

            QString $log_header_proc = $log_header.mid(18);
            QStringList $log_header_split = $log_header_proc.split(" at ");
            // CONSTRUCT: _with
            final->setWith($log_header_split.takeFirst());
            $log_header_proc = $log_header_split.join(" at ");
            $log_header_split = $log_header_proc.split(" on ");
            QString $date_proc = $log_header_split.takeFirst();
            $log_header_proc = $log_header_split.join(" on ");
            $log_header_split = $log_header_proc.split(" (");
            // CONSTRUCT: _self
            final->setSelf($log_header_split.takeFirst());
            $log_header_proc = $log_header_split.join(" (");
            $log_header_split = $log_header_proc.split(")");
            // CONSTRUCT: _protocol
            final->setProtocol($log_header_split.takeFirst());

            QStringList $date_split = $date_proc.split(" ");
            QString $timezone = $date_split.takeLast();
            $date_proc = $date_split.join(" ");
            QDateTime $time_proc;
            if ($date_proc.contains(QRegExp(" [AP]M$")))
                $time_proc = QDateTime::fromString($date_proc, "ddd dd MMM yyyy hh:mm:ss AP");
            else
                $time_proc = QDateTime::fromString($date_proc, "ddd dd MMM yyyy hh:mm:ss");
            if (!$time_proc.isValid())
            {
                qlonglong $datetime = interpretTime($date_proc);
                $time_proc = QDateTime::fromMSecsSinceEpoch($datetime);
            }
            // CONSTRUCT: _time
            $time_base = $time_proc.toMSecsSinceEpoch();
            initial_time = $time_base;
            final->setTime($time_base);

            $equizone = Helper::zone_search($timezone);
            // CONSTRUCT: _timezone
            final->setTimezone($equizone["full_tz_name"].toString());
        }

        // If token is a new chat row
        else if (msgstart_match.indexIn(line) > -1)
        {
            if (!msg_buffer.isEmpty())
            {
                // CONSTRUCT: _time
                final->setTime($time_base);
                // CONSTRUCT: _time_precision
                final->setPrecision(0);
                // CONSTRUCT: _sender
                final->setSender($sender_alias);
                // CONSTRUCT: _message
                final->setContent(msg_buffer);

                msg_buffer = "";
            }

            // Create new chat row
            final->newRow();
            // CONSTRUCT: _timezone
            final->setTimezone($equizone["full_tz_name"].toString());

            $time_base = interpretTime(msgstart_match.cap(0).trimmed(), $time_base);
            line = line.mid(msgstart_match.cap(0).length());
            QStringList line_split = line.split(": ");
            $sender_alias = line_split.takeFirst();

            /*// A probably not reliable way to guess the self alias
            if (linenum == 2 && $time_base == initial_time)
            {
                final->setSelfAlias($sender_alias);
            }
            if ($sender_alias != final->getSelfAlias() && final->getWithAlias().isEmpty())
            {
                final->setWithAlias($sender_alias);
            }*/

            msg_buffer += line_split.join(": ");
        }
        else
        {
            msg_buffer += "\n" + line;
        }
    }
    if (!msg_buffer.isEmpty())
    {
        // CONSTRUCT: _time
        final->setTime($time_base);
        // CONSTRUCT: _time_precision
        final->setPrecision(0);
        // CONSTRUCT: _sender
        final->setSender($sender_alias);
        // CONSTRUCT: _message
        final->setContent(msg_buffer);
    }
}

/**
 * Load an HTML Pidgin System Log
 */
void Pidgin::loadSystemHtml(QVariant $log_raw)
{
    // TODO
}

/**
 * Load a Plain Text Pidgin System Log
 */
void Pidgin::loadSystemPlainText(QVariant $log_raw)
{
    // TODO
}

/**
 * Try to Comprehend a Pidgin Timestamp
 * @param QString input The timestamp to attempt to interpret
 * @param qint64 $time_base The base time to compare to
 * @returns qint64 The interpreted timestamp in Log2Log format
 */
qlonglong Pidgin::interpretTime(QString input, qlonglong $time_base)
{
    // Remove parentheses and brackets
    input = input.replace("(", "")
            .replace(")", "")
            .replace("[", "")
            .replace("]", "");

    // There are about zero programming languages that have good timestamp
    // parsing capabilities. The Qt Framework on C++ is no exception.
    // What we have to do here is guess the format. This is very tedious,
    // and sometimes, it may even lead to false results! For example,
    // is 01/02/2003 January 02, 2003 or February 01, 2003?
    //   -- Deltik

    // Define variables that may be used.
    qlonglong time_sum = 0;
    QTime time_proc;
    QDate date_proc;

    // Break up the string so the bits are easier to look at.
    QStringList split = input.split(" ");
    int parts_count = split.count();

    // TIME GUESSER
    // Try to match for ":"
    int timePos;
    for (timePos = 0; timePos < parts_count; timePos ++)
        if (split[timePos].contains(":"))
            break;
    // Guess the time, trying various possible formats.
    if (timePos < parts_count && split[timePos].contains(":"))
    {
        time_proc = QTime::fromString(split[timePos], "hh:mm:ss");
        if (!time_proc.isValid())
            time_proc = QTime::fromString(split[timePos], "h:mm:ss");
        if (!time_proc.isValid())
            time_proc = QTime::fromString(split[timePos], "hh:mm");
        if (!time_proc.isValid())
            time_proc = QTime::fromString(split[timePos], "h:mm");
        if (!time_proc.isValid())
            time_proc = QTime::fromString(split[timePos], "hh:mm:ss.zzz");
        // Ridiculous hotfix for handling 12:00:00 PM to 12:59:59 PM
        if (timePos + 1 < split.size() && time_proc.hour() == 12 &&
                (
                    split[timePos+1].toLower().contains("am") ||
                    split[timePos+1].toLower().contains("a.m") ||
                    split[timePos+1].toLower().contains("a m") ||
                    split[timePos+1].toLower().contains("ante")
                )
           )
            time_proc = time_proc.addSecs(-43200);
        // Add to the final sum
        time_sum += -time_proc.msecsTo(QTime(0,0,0,0));
    }

    // MERIDIEM ADDER
    // Try to match for "post meridiem"
    // If matches, add 12 hours (43200000 milliseconds) to the final sum
    if (time_proc.hour() != 12 &&
            (
                input.toLower().contains("pm") ||
                input.toLower().contains("p.m") ||
                input.toLower().contains("p m") ||
                input.toLower().contains("post")
            )
        )
        time_sum += 43200000;

    // DATE GUESSER
    // Try to match for "/"
    bool matchedDate = false;
    int datePos;
    for (datePos = 0; datePos < parts_count; datePos ++)
        if (split[datePos].contains("/"))
            break;
    // Guess the time, trying various possible formats and doing a weak
    // confirmation by checking with $time_base to make sure that we're
    // not going backwards in time.
    if (datePos < parts_count && split[datePos].contains("/"))
    {
        // Try locale first. **crosses fingers**
        QLocale locale;
        QString expectedFormat = locale.dateFormat(QLocale::ShortFormat).toLower();
        QStringList expectParts = expectedFormat.split("/");
        bool forceEurope = false;
        bool forceIntl   = false;
        // Month first (MM/DD/YYYY)
        // Always try this first, regardless of locale.
        if (true)
        {
            date_proc = QDate::fromString(split[datePos], "MM/dd/yyyy");
            if (!date_proc.isValid())
                date_proc = QDate::fromString(split[datePos], "M/d/yyyy");
            if (!date_proc.isValid())
                date_proc = QDate::fromString(split[datePos], "M/dd/yyyy");
            if (!date_proc.isValid())
                date_proc = QDate::fromString(split[datePos], "MM/d/yyyy");
            if (!date_proc.isValid())
                date_proc = QDate::fromString(split[datePos], "MM/dd/yy");
            if (!date_proc.isValid())
                date_proc = QDate::fromString(split[datePos], "M/d/yy");
            if (!date_proc.isValid())
                date_proc = QDate::fromString(split[datePos], "M/dd/yy");
            if (!date_proc.isValid())
                date_proc = QDate::fromString(split[datePos], "MM/d/yy");
            // Check for backwards time travel
            QDate comparer = QDate::fromString(QDateTime::fromMSecsSinceEpoch($time_base)
                                               .toString("yyyy/MM/dd"),
                                               "yyyy/MM/dd");
            // If backwards time travel detected or the date still isn't valid,
            // try day first (DD/MM/YYYY)
            if (comparer > date_proc ||
                !date_proc.isValid())
                forceEurope = true;
        }
        if (expectParts[0].left(1) == "d" || forceEurope)
        {
            date_proc = QDate::fromString(split[datePos], "dd/MM/yyyy");
            if (!date_proc.isValid())
                date_proc = QDate::fromString(split[datePos], "d/M/yyyy");
            if (!date_proc.isValid())
                date_proc = QDate::fromString(split[datePos], "d/MM/yyyy");
            if (!date_proc.isValid())
                date_proc = QDate::fromString(split[datePos], "dd/M/yyyy");
            if (!date_proc.isValid())
                date_proc = QDate::fromString(split[datePos], "dd/MM/yy");
            if (!date_proc.isValid())
                date_proc = QDate::fromString(split[datePos], "d/M/yy");
            if (!date_proc.isValid())
                date_proc = QDate::fromString(split[datePos], "d/MM/yy");
            if (!date_proc.isValid())
                date_proc = QDate::fromString(split[datePos], "dd/M/yy");
            // Check for backwards time travel
            QDate comparer = QDate::fromString(QDateTime::fromMSecsSinceEpoch($time_base)
                                               .toString("yyyy/MM/dd"),
                                               "yyyy/MM/dd");
            // If backwards time travel detected or the date still isn't valid,
            // try year first (YYYY/MM/DD)
            if (comparer > date_proc ||
                !date_proc.isValid())
                forceIntl = true;
        }
        if (expectParts[0].left(1) == "y" || forceIntl)
        {
            date_proc = QDate::fromString(split[datePos], "yyyy/MM/dd");
            if (!date_proc.isValid())
                date_proc = QDate::fromString(split[datePos], "yy/M/d");
            if (!date_proc.isValid())
                date_proc = QDate::fromString(split[datePos], "yy/MM/dd");
            if (!date_proc.isValid())
                date_proc = QDate::fromString(split[datePos], "yy/M/dd");
            if (!date_proc.isValid())
                date_proc = QDate::fromString(split[datePos], "yy/MM/d");
            if (!date_proc.isValid())
                date_proc = QDate::fromString(split[datePos], "yyyy/M/d");
            if (!date_proc.isValid())
                date_proc = QDate::fromString(split[datePos], "yyyy/M/dd");
            if (!date_proc.isValid())
                date_proc = QDate::fromString(split[datePos], "yyyy/MM/d");
            // If there is still backwards time travel, too bad.
            QDate comparer = QDate::fromString(QDateTime::fromMSecsSinceEpoch($time_base)
                                               .toString("yyyy/MM/dd"),
                                               "yyyy/MM/dd");
            // If the date still isn't valid, too bad. Steal from $time_base.
            if (!date_proc.isValid())
                date_proc = comparer;
        }
        // Add to the final sum
        if (date_proc.isValid())
        {
            time_sum += QDateTime(date_proc).toMSecsSinceEpoch();
            matchedDate = true;
        }
    }

    // One last check for backwards time travel...
    if (!matchedDate)
    {
        time_sum += QDateTime::fromString(QDateTime::fromMSecsSinceEpoch($time_base)
                                          .toString("yyyy/MM/dd"),
                                          "yyyy/MM/dd").toMSecsSinceEpoch();
        QDateTime comparer = QDateTime::fromMSecsSinceEpoch($time_base);
        // If the new time is before $time_base, advance to the next day
        // (+86400000 milliseconds) (24 hours).
        if (comparer > QDateTime::fromMSecsSinceEpoch(time_sum))
        {
            time_sum += 86400000;
        }
    }

    // After MUCH exhaustive work, we FINALLY return the interpreted time!
    return time_sum;
}

/**
 * Return the Specified Line
 */
QString Pidgin::readLine(QString text, qint64 line)
{
    QStringList text_split = text.split("\n");
    return text_split[(int)line - 1];
}

/**
 * Generate Log from Standardized Log
 * TODO: Finish
 */
QVariant Pidgin::generate(StdFormat *$log)
{
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
        // Chat log header
        QString $content = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\"><html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"><title>";

        // Put the longer variables into something more readily accessible.
        QString $protocol      = $log->getProtocol();
        QString $account       = $log->getSelf();
        QString $self_alias    = $log->getSelfAlias();
        QString $with          = $log->getWith();
        QString $with_alias    = $log->getWithAlias();
        qlonglong $time_base   = $log->getTime();
        QString $timezone_base = $log->getTimezone();

        // START PATCH: $protocol: "gtalk" => "jabber"
        $protocol = $protocol.replace("gtalk", "jabber");
        // END PATCH

        // <title> Setup
        QMap<QString, QVariant> equizone = Helper::zone_search($timezone_base);
        QString $timezone_abbreviation   = equizone["name"].toString();
        int $timezone_offset             = equizone["gmtoffset"].toInt();
        QTime $tz_offset                 = QTime::fromMSecsSinceStartOfDay($timezone_offset * 1000);
        QString negative;
        if ($timezone_offset < 0) negative = "-"; else negative = "+";
        QString $tz_offset_string        = negative + QVariant($tz_offset.hour()).toString().rightJustified(2, '0') + QVariant($tz_offset.minute()).toString().rightJustified(2, '0');
        QString $relname       = $protocol + "/" +
                $account +
                "/" +
                $with +
                "/" +
                QDateTime::fromMSecsSinceEpoch($time_base).toString("yyyy-MM-dd.hhmmss") +
                $tz_offset_string +
                $timezone_abbreviation.toUpper() +
                ".html";
        QLocale la; QString tmp = la.dateTimeFormat(QLocale::ShortFormat);
        QString $autoAP;
        if (tmp.toLower().contains("ap"))
            $autoAP = " AP";
        QString $datetimestamp = QDateTime::fromMSecsSinceEpoch($time_base).toString("ddd dd MMM yyyy hh:mm:ss"+$autoAP+" ");
        QString title = "Conversation with " +
                        $with +
                        " at " +
                        $datetimestamp +
                        $timezone_abbreviation.toUpper() +
                        " on " +
                        $account +
                        " (" +
                        $protocol +
                        ")";

        // <title>
        $content += title + "</title></head><body><h1>" + title + "</h1><p>\n";

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

            // Use sender as alias if alias is empty
            if ($alias.isEmpty()) $alias = $sender;

            // Color Legend:
            //  #16569E : _self
            //  #A82F2F : _with
            //  #FF0000 : _evt_error
            //  #6C2585 : Whisper (not supported yet)
            //  #062585 : /me
            QString $sender_color;

            // Timestamp (hh:mm:ss [AP])
            QString $timestamp;
            if ($precision == 2)
                $timestamp = QDateTime::fromMSecsSinceEpoch($time_cur)
                                     .toString("hh:mm"+$autoAP);
            else
                $timestamp = QDateTime::fromMSecsSinceEpoch($time_cur)
                                     .toString("hh:mm:ss"+$autoAP);
            if (QDateTime::fromString(QDateTime::fromMSecsSinceEpoch($time_base).toString("yyyy/MM/dd")
                                      , "yyyy/MM/dd").toMSecsSinceEpoch() !=
                QDateTime::fromString(QDateTime::fromMSecsSinceEpoch($time_cur).toString("yyyy/MM/dd")
                                      , "yyyy/MM/dd").toMSecsSinceEpoch())
                $timestamp = QDateTime::fromMSecsSinceEpoch($time_cur)
                .toString("yyyy/MM/dd hh:mm:ss"+$autoAP);

            // If we're looking at a system message...
            if ($code >= 1)
            {
                // Color: Error
                if ($sender.contains("_evt_fail") ||
                    $sender.contains("_error"))
                    $sender_color = "#FF0000";

                // To Pidgin Notices
                // TODO
                if ($sender.contains("_evt_online"))
                    $message = $alias + " has signed on.";

                if ($sender_color.isEmpty())
                    $content += "<span style=\"font-size: smaller\">(" +
                                $timestamp +
                                ")</span><b> " +
                                $message +
                                "</b><br>\n";
                else
                    $content += "<span style=\"color: " +
                                $sender_color +
                                "\"><span style=\"font-size: smaller\">(" +
                                $timestamp +
                                ")</span><b> " +
                                $message +
                                "</b><br>\n";
            }
            // Color: Sent by _self
            if ($sender == "_self" ||
                $sender == $account ||
                $sender == $self_alias)
                $sender_color = "#16569E";
            // Color: Sent by _self
            else if (!$sender.contains(QRegExp("^_(evt_|group_|error)")))
                $sender_color = "#A82F2F";

            if (!$sender_color.isEmpty())
            {
                $content += "<span style=\"color: " +
                            $sender_color +
                            "\"><span style=\"font-size: smaller\">(" +
                            $timestamp +
                            ")</span> <b>" +
                            $alias +
                            ":</b></span> " +
                            $message +
                            "<br>\n";
            }

            // Closing
            if (!$log->hasNextRow())
                $content += "</p>\n</body>\n</html>\n";
        }//qDebug() << $content;
        $info["content"] = $content;
        $info["modtime"] = $time_base / 1000;
        //qDebug() << $relname;
        $log_new[$relname] = $info;

        // Increment the entry key.
        $i ++;
        // Update the progress bar.
        updateProgress((40 * $i / total) + 50, "Converted " + QVariant($i).toString() + "/" + QVariant(total).toString() + " files...");
    }
    $log_generated = $log_new;
    return $log_generated;
}

/**
 * Process "From" Request
 */
StdFormat* Pidgin::from(QHash<QString, QVariant> data)
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
    // Run the Log2Log Postprocessor to guess or try to fill in missing data.
    updateProgress(50, "Filling in data gaps...");
    Helper::postprocessor(final);

    // Step 3/3: Submit the Log2Log-standardized chat log array.
    emit finished();
    return this->final;
}

/**
 * Process "To" Request
 */
void Pidgin::to(StdFormat* $log)
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
