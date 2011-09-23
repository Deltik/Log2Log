/**
 * Log2Log
 *  Formats
 *   Meebo (files)
 *
 * @author igordcard
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
    final->setClient("Meebo");
}

/**
 * Load a Chat Log
 */
void Meebo::load(QVariant $log_raw)
{
    QString $log_refined;
    QString $log_date;
    QStringList $log_chats;
    QStringList $log_each_item;
    QByteArray *$log_date_bytes = new QByteArray();

    // Fix whitespace characters recognition
    $log_refined = $log_raw.toString()
            .replace("\\t", "\t")
            .replace("\\n", "\n")
            .replace("\\r", "\r")
            .replace("\\0", "\0")
            .replace("\\x0B", "\x0B");

    // Strip the crufty slashes inside the Meebo file
    $log_refined = Helper::stripslashes($log_refined);

    // Trim log string: '\t', '\n', '\v', '\f', '\r', and ' '
    $log_refined.trimmed();

    // Trim log string: '"'
    $log_refined.replace(0, 1, "");
    $log_refined.replace($log_refined.length()-1, 1, "");

    // Split chat instances
    $log_chats = $log_refined.split("<br/><hr size=1><div class='ImChatHeader'>");
    $log_chats.removeFirst();

    // Retrieve Meebo log entries' start times
    foreach(QString $log_item, $log_chats) {
        $log_each_item.clear();
        $log_date.clear();
        $log_date_bytes->clear();
        $log_each_item = $log_item.split("</div><hr size=1>");
        $log_date = $log_each_item.first();
        // HTML entity decode
        $log_date = QUrl::fromPercentEncoding($log_date_bytes->append($log_date.toUtf8()));
        // TODO: standardize $log_date to time
    }
}

/**
 * Generate Log from Standardized Log
 */
QVariant Meebo::generate(StdFormat *$log)
{
    // TODO
}

/**
 * MEEBO_CUSTOM: Set Account
 */
void Meebo::setAccount(QString account)
{
    /* PATCH FOR REMOVING "/Meebo" RESOURCE TAG */
    if (account.toLower().endsWith("/meebo"))
        account.chop(6);
    /* END PATCH */
    $account = account;
}

/**
 * MEEBO_CUSTOM: Set IM Protocol
 */
void Meebo::setProtocol(QString protocol)
{
    $protocol = protocol;
}

/**
 * MEEBO_CUSTOM: Set Other User's Account
 */
void Meebo::setWith(QString with)
{
    $with = with;
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

        // Get file path
        QString filepath = i.key();
        // Remove file extension
        QStringList path_parts = filepath.split(".");
        path_parts.pop_back();
        filepath = path_parts.join(".");
        // Get Log2Log Meebo metadata
        path_parts = filepath.split("|");
        QString account  = path_parts[0];
        QString protocol = path_parts[1];
        QString with     = path_parts[2];

        // Set metadata
        this->setAccount(account);
        this->setProtocol(protocol);
        this->setWith(with);

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
    while ($log->nextEntry())
    {
        total ++;
        updateProgress(50, "Counting Log Entries... ("+QVariant(total).toString()+" found so far)");
    }

    data = this->generate($log);
    emit finished();
}
