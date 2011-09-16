/**
 * Log2Log
 *  Formats
 *   Log2Log JSON
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

#include "stdjson.h"
#include "helper.h"

StdJson::StdJson()
{
    final = new StdFormat();
}

/**
 * Load a Chat Log
 */
void StdJson::load(QVariant $log_raw)
{
    // TODO
}

/**
 * Generate Log from Standardized Log
 */
QVariant StdJson::generate(StdFormat *$log)
{
    // TODO
}
/**
 * Process "From" Request
 */
StdFormat* StdJson::from(QHash<QString, QVariant> data)
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
void StdJson::to(StdFormat* $log)
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
