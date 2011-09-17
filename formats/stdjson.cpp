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
#include "json.h"
#include "helper.h"
#include "update.h"

StdJson::StdJson()
{
    final = new StdFormat();
}

/**
 * Load a Chat Log
 */
void StdJson::load(QVariant $log_raw)
{
    bool logIsJson = false;
    updateProgress(30, "Importing... (Be patient)");
    QMap<QString, QVariant> log = Json::parse($log_raw.toString(), logIsJson).toMap();

    if (logIsJson)
        final->log = log;
}

/**
 * Generate Log from Standardized Log
 */
QVariant StdJson::generate(StdFormat *$log)
{
    QVariant $log_generated;
    QMap<QString, QVariant> $log_new;
    QHash<QString, QVariant> $info;

    updateProgress(70, "Exporting... (Be patient)");

    $info["content"] = Json::serialize($log->log);
    $info["modtime"] = QDateTime::currentMSecsSinceEpoch() / 1000;

    $log_new[QDateTime::fromMSecsSinceEpoch(QDateTime::currentMSecsSinceEpoch()).toString(Qt::ISODate) + ".json"] = $info;
    $log_generated = $log_new;

    return $log_generated;
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
    while ($log->nextEntry())
        {
            total ++;
            updateProgress(50, "Counting Log Entries... ("+QVariant(total).toString()+" found so far)");
        }

    data = this->generate($log);
    emit finished();
}
