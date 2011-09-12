/**
 * Log2Log Online Chat Log Converter
 *  Formats
 *   Meebo (files)
 *
 * @author Deltik
 */

#include "meebo.h"
#include "helper.h"
#include <QtCore>
#include <QtXml>
#include <QDateTime>

/**
 * Constructor
 */
Meebo::Meebo()
{
    final = new StdFormat();
    final->setClient("Omegle");
}

/**
 * Load a Chat Log
 */
void Meebo::load(QVariant $log_raw)
{
    // TODO
}

/**
 * Generate Log from Standardized Log
 */
QVariant Meebo::generate(StdFormat *$log)
{
    // TODO
}

/**
 * Process "From" Request
 */
StdFormat* Meebo::from(QHash<QString, QVariant> data)
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
void Meebo::to(StdFormat* $log)
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
