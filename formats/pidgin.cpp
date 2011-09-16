/**
 * Log2Log
 *  Formats
 *   Pidgin HTML
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
    if ($log_raw.toString().left(6) == "<html>")
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
    $log_proc = $log_split.join("</head>");
    $log_proc.replace("</body></html>", "</body>");

    // Create the HTML reader.
    QXmlStreamReader xml($log_proc);

    // Read the HTML file.
    while (!xml.atEnd())
    {
        // Read next item
        QXmlStreamReader::TokenType token = xml.readNext();

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
            // If token is the log meta indicator...
            if (xml.qualifiedName().toString() == "h3")
            {
                // Point to the data.
                xml.readNext();

                // Unfortunately, RegExp can't help with this tricky next part.
                // Here, we look at Pidgin's chat log or system log header:
                //  - "Conversation with _WITH at _DAY_OFWEEK_3LETTERS _DAY_2DIGITS _MONTH_3LETTERS _YEAR_4DIGITS _TIME_BASE _TIMEZONE_ABBREVIATION on _SELF (_PROTOCOL)"
                //  - "System log for account _SELF (_PROTOCOL) connected at _DAY_OFWEEK_3LETTERS _DAY_2DIGITS _MONTH_3LETTERS _YEAR_4DIGITS _TIME_BASE _TIMEZONE_ABBREVIATION"
                QString $log_header = xml.text().toString();
                // If this is a system log, forward to the proper method.
                if ($log_header.left(6) == "System")
                    return loadSystemHtml($log_raw);

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
                int $date_parts_count = $date_split.count();
                QString $timezone = $date_split.takeLast();
                $date_proc = $date_split.join(" ");
                QDateTime $time_proc;
                if ($date_parts_count == 7)
                    $time_proc = QDateTime::fromString($date_proc, "ddd dd MMM yyyy hh:mm:ss AP");
                else
                    $time_proc = QDateTime::fromString($date_proc, "ddd dd MMM yyyy hh:mm:ss");
                // CONSTRUCT: _time
                $time_base = $time_proc.toMSecsSinceEpoch();
                final->setTime($time_base);

                $equizone = Helper::zone_search($timezone);
                // CONSTRUCT: _timezone
                final->setTimezone($equizone["full_tz_name"].toString());
            }

            // If token is a chat row
            if (xml.qualifiedName().toString() == "font")
            {
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
                QString $sender_color = xml.attributes().value("color").toString();

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
                QStringList $line_split = $line.split(")</font>");
                $line_split.pop_front();
                $line = $line_split.join(")</font>").trimmed();

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
                            // CONSTRUCT: _alias
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
                    if ($line.right(5) == "<br/>")
                        $line.chop(5);
                    $line = $line.trimmed();
                    if ($line.left(7) == "</font>")
                        $line = $line.mid(7).trimmed();
                    $line = $line.replace("&quot;", "\"");
                    $line = $line.replace("&amp;", "&");
                    // CONSTRUCT: _message
                    final->setContent($line);
                }
            }
        }
    }

    //qDebug()<<final->final;
}

/**
 * Load a Plain Text Pidgin Chat Log
 */
void Pidgin::loadPlainText(QVariant $log_raw)
{
    // TODO
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
    // Trim input of parentheses
    if (input.left(1) == "(")
        input = input.mid(1);
    if (input.right(1) == ")")
        input = input.mid(0, input.length() - 1);

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
        // Add to the final sum
        time_sum += -time_proc.msecsTo(QTime());
    }

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
        time_sum += QDateTime(date_proc).toMSecsSinceEpoch();
        if (date_proc.isValid())
            matchedDate = true;
    }

    // One last check for backwards time travel...
    if (!matchedDate)
    {
        QDateTime comparer = QDateTime::fromMSecsSinceEpoch($time_base);
        // If the new time is before $time_base, advance to the next day
        // (+86400000 milliseconds) (24 hours).
        if (-comparer.time().msecsTo(QTime()) > -time_proc.msecsTo(QTime()))
        {
            time_sum = -time_proc.msecsTo(QTime()) +
                       QDateTime::fromString(QDateTime::fromMSecsSinceEpoch($time_base)
                                             .toString("yyyy/MM/dd"),
                                             "yyyy/MM/dd").toMSecsSinceEpoch();
        }
    }

    // MERIDIEM ADDER
    // Try to match for "post meridiem"
    // If matches, add 12 hours (43200000 milliseconds) to the final sum
    if (input.toLower().contains("pm") ||
        input.toLower().contains("p.m") ||
        input.toLower().contains("p m") ||
        input.toLower().contains("post"))
        time_sum += 43200000;

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
        QString $content = "<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\"><title>";

        // Put the longer variables into something more readily accessible.
        QString $protocol      = $log->getProtocol();
        QString $account       = $log->getSelf();
        QString $self_alias    = $log->getSelfAlias();
        QString $with          = $log->getWith();
        QString $with_alias    = $log->getWithAlias();
        qlonglong $time_base   = $log->getTime();
        QString $timezone_base = $log->getTimezone();

        // <title> Setup
        /** * * * * * * * * * * * * * * * * * * *
         *   THIS IS THE VERY FIRST THING YOU   *
         *   NEED TO WORK ON... TOP PRIORITY!   *
         * _________ _______  ______   _______  *
         * \__   __/(  ___  )(  __  \ (  ___  ) *
         *    ) (   | (   ) || (  \  )| (   ) | *
         *    | |   | |   | || |   ) || |   | | *
         *    | |   | |   | || |   | || |   | | *
         *    | |   | |   | || |   ) || |   | | *
         *    | |   | (___) || (__/  )| (___) | *
         *    )_(   (_______)(______/ (_______) *
         *                                      *
         * #################################### *
         * #  SUPER-MEGA-HYPER PRIORITY TODO  # *
         * #################################### *
         *                                      *
         * TODO: Get timezones to work.         *
         ** * * * * * * * * * * * * * * * * * * */
        QMap<QString, QVariant> equizone = Helper::zone_search($timezone_base);
        QString $timezone_abbreviation = equizone["name"].toString();
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
        $content += title + "</title></head><body><h3>" + title + "</h3>\n";

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

            // Color Legend:
            //  #16569E : _self
            //  #A82F2F : _with
            //  #FF0000 : _evt_error
            //  #6C2585 : Whisper (not supported yet)
            //  #062585 : /me
            QString $sender_color;

            // Timestamp (hh:mm:ss [AP])
            QString $timestamp = QDateTime::fromMSecsSinceEpoch($time_cur)
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
                    $content += "<font size=\"2\">(" +
                                $timestamp +
                                ")</font><b> " +
                                $message +
                                "</b><br/>\n";
                else
                    $content += "<font color=\"" +
                                $sender_color +
                                "\"><font size=\"2\">(" +
                                $timestamp +
                                ")</font><b> " +
                                $message +
                                "</b><br/>\n";
            }
            // Color: Sent by _self
            if ($sender == "_self" ||
                $sender == $account ||
                $sender == $self_alias)
                $sender_color = "#16569E";
            // Color: Sent by _self
            if ($sender == "_with" ||
                $sender == $with ||
                $sender == $with_alias)
                $sender_color = "#A82F2F";

            if (!$sender_color.isEmpty())
            {
                $content += "<font color=\"" +
                            $sender_color +
                            "\"><font size=\"2\">(" +
                            $timestamp +
                            ")</font> <b>" +
                            $alias +
                            ":</b></font> " +
                            $message +
                            "<br/>\n";
            }

            // Closing
            if (!$log->hasNextRow())
                $content += "</body></html>";
        }qDebug() << $content;
    }
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
    if ($log->gotoEntry(0) == false)
        total = 0;
    else
    {
        do
        {
            total ++;
            updateProgress(50, "Counting Log Entries... ("+QVariant(total).toString()+" found so far)");
        }
        while ($log->nextEntry());
    }

    data = this->generate($log);
    emit finished();
}
