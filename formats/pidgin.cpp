/**
 * Log2Log Online Chat Log Converter
 *  Formats
 *   Pidgin HTML
 *
 * @author Deltik
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
        qlonglong $time_base;

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

                QMap<QString, QVariant> $equizone = Helper::zone_search($timezone);
                // CONSTRUCT: _timezone
                final->setTimezone($equizone["full_tz_name"].toString());
            }

            // If token is a chat row
            if (xml.qualifiedName().toString() == "font")
            {
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
                    final->setTime($time_base);
                }
            }
        }
    }qDebug()<<final->final;
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
 * Generate Log from Standardized Log
 */
QVariant Pidgin::generate(StdFormat *$log)
{
    // TODO
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
