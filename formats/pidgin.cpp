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
                final->setTime($time_proc.toMSecsSinceEpoch());

                QMap<QString, QVariant> $equizone = Helper::zone_search($timezone);
                // CONSTRUCT: _timezone
                final->setTimezone($equizone["full_tz_name"].toString());
            }
        }
    }
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
