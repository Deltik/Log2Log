#include "helper.h"
#include <QtGui/QLayout>
#include <QGridLayout>
#include <QDirIterator>
#include <QTextStream>
#include "log2log.h"
#include "ui_log2log.h"
#include <QDebug>

/* This class assumes and requires the existence
 * of log2log.ui which comes with Log2Log */

Helper::Helper()
{
}

/**
 * Bed Intruder: Hide Yo Kids: QGridLayout Edition
 */
void Helper::hide(QGridLayout *ql)
{
    for (int i = 0; i < ql->count(); i ++)
        ql->itemAt(i)->widget()->hide();
}

/**
 * Bed Intruder: Hide Yo Kids: QVBoxLayout Edition, namely the "PathLayout"s
 */
void Helper::hide(QVBoxLayout *ql)
{
    ql->itemAt(0)->widget()->hide();
    ql->itemAt(1)->layout()->itemAt(0)->widget()->hide();
    ql->itemAt(1)->layout()->itemAt(1)->widget()->hide();
}

/**
 * Show Items
 */
void Helper::show(QGridLayout *ql)
{
    for (int i = 0; i < ql->count(); i ++)
        ql->itemAt(i)->widget()->show();
}

/**
 * Show Items, namely the "PathLayout"s
 */
void Helper::show(QVBoxLayout *ql)
{
    ql->itemAt(0)->widget()->show();
    ql->itemAt(1)->layout()->itemAt(0)->widget()->show();
    ql->itemAt(1)->layout()->itemAt(1)->widget()->show();
}

/**
 * Switch to Convert Mode
 * @param QWidget parent The MainWindow to edit
 */
void Helper::convertMode(Ui::Log2Log *parent)
{
    parent->srcProtoBox->setEnabled(false);
    parent->dstProtoBox->setEnabled(false);
    parent->srcPathEdit->setEnabled(false);
    parent->dstPathEdit->setEnabled(false);
    parent->srcPathButton->setEnabled(false);
    parent->dstPathButton->setEnabled(false);
    parent->srcUserEdit->setEnabled(false);
    parent->dstUserEdit->setEnabled(false);
    parent->srcPassEdit->setEnabled(false);
    parent->dstPassEdit->setEnabled(false);

    parent->convertButton->setIcon(QIcon());
    parent->convertButton->setIcon(QIcon::fromTheme("process-stop", QIcon(":/images/etc/cancel.png")));
}

/**
 * Switch to Main Mode
 * @param QWidget parent The MainWindow to edit
 */
void Helper::mainMode(Ui::Log2Log *parent)
{
    parent->srcProtoBox->setEnabled(true);
    parent->dstProtoBox->setEnabled(true);
    parent->srcPathEdit->setEnabled(true);
    parent->dstPathEdit->setEnabled(true);
    parent->srcPathButton->setEnabled(true);
    parent->dstPathButton->setEnabled(true);
    parent->srcUserEdit->setEnabled(true);
    parent->dstUserEdit->setEnabled(true);
    parent->srcPassEdit->setEnabled(true);
    parent->dstPassEdit->setEnabled(true);

    parent->convertButton->setIcon(QIcon::fromTheme("process-start", QIcon(":/images/etc/Arrow.png")));
}

/**
 * Log2Log Timestamp Precision Index Interpreter
 * @author Deltik
 */
QString Helper::whatPrecision(int index)
{
    switch (index)
    {
    case -3: return "nanosecond";
    case -2: return "microsecond";
    case -1: return "millisecond";
    case 0: return "second";
    case 1: return "tenth second";
    case 2: return "minute";
    case 3: return "tenth minute";
    case 4: return "hour";
    case 5: return "tenth hour";
    case 6: return "day";
    case 7: return "week of a date";
    case 8: return "month of a date";
    case 9: return "year of a date";
    case 10: return "decade of a date";
    case 11: return "century of a date";
    case 12: return "millenium of a date";
    default: return "never. Time isn't supported by this";
    }
}

/**
 * Log2Log Trinary to String Converter
 * @author Deltik
 */
QString Helper::whatTrinary(int bit)
{
    switch (bit)
    {
    case 0: return "not stored at all";
    case 1: return "stored in groups of messages";
    case 2: return "stored with each message";
    default: return "not stored at all";
    }
}

/**
 * Log2Log Generic Chat Log Format Postprocessor
 * @author Deltik
 */
void Helper::postprocessor(StdFormat *$log)
{
    /// Log2Log Postprocessor makes two runs through the log data:
    ///  1. Creation of a massive alias-to-sender association array to make it
    ///     convenient to look up the sender when only the alias is present
    ///  2. Defaulting to fill in gaps, including the ones fillable from the
    ///     alias-to-sender array.

    // Browser
    $log->resetPointer();

    // Artificially Intelligent Information Tracker
    QHash<QString, QString> people;

    // CREATE: Alias-Account Association Array
    while ($log->nextEntry())
    {
        QString $account       = $log->getSelf();
        QString $self_alias    = $log->getSelfAlias();
        QString $with          = $log->getWith();
        QString $with_alias    = $log->getWithAlias();

        while ($log->nextRow())
        {
            QString $sender      = $log->getSender();
            QString $alias       = $log->getAlias();

            // DEFAULT: _sender
            if ($sender.left(5) == "_with" && !$with.isEmpty())
                $log->setSender($with);
            if ($sender.left(5) == "_self" && !$account.isEmpty())
                $log->setSender($account);
            // If there's no information about the sender...
            if (($sender.isEmpty() || $sender == "_unknown") && $alias.isEmpty())
            {
                $log->setSender("_unknown");
            }
            // Pull sender from global with or global self
            else if (($sender.isEmpty() || $sender == "_unknown") && !$alias.isEmpty())
            {
                if ($alias == $self_alias && !$account.isEmpty())
                    $log->setSender($account);
                if ($alias == $with_alias && !$with.isEmpty())
                    $log->setSender($with);
            }

            // ADD: Alias-Account Association Array
            if (!$sender.contains("_unknown") && !$alias.contains("_unknown") &&
                !$sender.isEmpty() && !$alias.isEmpty())
            {
                people.insert($alias, $account);
            }
        }
    }

    // Start Over
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

        // DEFAULT: _protocol
        if ($protocol.isEmpty())
            $log->setProtocol("unknown");

        while ($log->nextRow())
        {
            // Make array items more readily accessible.
            qlonglong $time_cur  = $log->getTime();
            QString $timezone    = $log->getTimezone();
            int     $code        = $log->getCode();
            QString $sender      = $log->getSender();
            QString $alias       = $log->getAlias();
            QString $message     = $log->getContent();
            int     $precision   = $log->getPrecision();
            int     $accuracy    = $log->getAccuracy();
            int     $nice        = $log->getNice();

            // DEFAULT: _time
            if (!$time_cur)
                $log->setTime($time_base);

            // DEFAULT: _time_zone
            if ($timezone.isEmpty())
                $log->setTimezone($timezone_base);

            // DEFAULT: _code
            if (!$code)
                $log->setCode(0);

            // DEFAULT: _precision
            if (!$precision)
                $log->setPrecision(0);

            // DEFAULT: _accuracy
            if (!$accuracy)
                $log->setAccuracy(0);

            // DEFAULT: _nice
            if (!$nice)
                $log->setNice(0);

            // FILL: _sender
            if (($sender.isEmpty() || $sender == "_unknown") && !$alias.isEmpty())
            {
                if (!people.value($alias).isNull())
                    $log->setSender(people.value($alias));
            }
        }
    }
}

/**
 * Get Multiple Files' Contents
 * @returns QMap<QString, QVariant> where QString is the filename and QVariant
 *          is the file contents
 */
QMap<QString, QVariant> Helper::files_get_contents(QString directory_path)
{
    // List of files like: list[FILENAME] = FILE_CONTENTS;
    QMap<QString, QVariant> list;

    // QDirIterator goes through files recursively in a directory. :)
    QDirIterator directory_walker(directory_path, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);

    // While directory_walker has another file to go through
    while (directory_walker.hasNext())
    {
        // Next!
        directory_walker.next();

        // Load File
        QString fileContents = "";
        QFile file(directory_walker.filePath());
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream in(&file);
        while (!in.atEnd())
        {
            QString line = in.readLine();
            if (!in.atEnd())
                fileContents += line + "\n";
            else
                fileContents += line;
        }
        list[directory_walker.filePath()] = fileContents;
    }

    // Return the list that this method made. :D
    return list;
}

/**
 * Returns associative array containing dst, offset and the timezone name
 * @remarks The table <resources/timezonemap.h> is directly exported from
 *          PHP.
 * @returns QList<QMap<QString, QVariant> >
 */
QList<QMap<QString, QVariant> > Helper::timezone_abbreviations_list()
{
    typedef struct _timelib_tz_lookup_table {
            QString     name;
            int         type;
            float       gmtoffset;
            QString     full_tz_name;
    } timelib_tz_lookup_table;

    timelib_tz_lookup_table timelib_timezone_lookup[] = {
        #include "resources/timezonemap.h"
        { NULL, 0, 0, NULL },
    };

    int i = 0;
    QList<QMap<QString, QVariant> > list;
    while (!timelib_timezone_lookup[i].name.isNull())
    {
        QMap<QString, QVariant> map;
        map["name"] = timelib_timezone_lookup[i].name;
        map["type"] = timelib_timezone_lookup[i].type;
        map["gmtoffset"] = timelib_timezone_lookup[i].gmtoffset;
        map["full_tz_name"] = timelib_timezone_lookup[i].full_tz_name;
        list << map;
        i ++;
    }

    return list;
}

/**
 * Timezone Search
 * @returns QMap<QString, QVariant>
 */
QMap<QString, QVariant> Helper::zone_search(QVariant query)
{
    QList<QMap<QString, QVariant> > list = timezone_abbreviations_list();

    for (int i = 0; i < list.count(); i ++)
    {
        QMap<QString, QVariant> map_cur = list[i];
        if (map_cur["name"].toString().toLower()         == query.toString().toLower() ||
            map_cur["full_tz_name"].toString().toLower() == query.toString().toLower())
            return map_cur;
    }

    return QMap<QString, QVariant>();
}
