/**
 * Log2Log
 *  Formats
 *   AIM
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

#include "aim.h"
#include "helper.h"

Aim::Aim()
{
    final = new StdFormat();
    final->setClient("AIM");
}

/**
 * Load a Chat Log
 */
void Aim::load(QVariant $log_raw)
{
    // If $log_raw does not contain an AIM chat log...
    if (!$log_raw.toString().startsWith("<?xml version=\"1.0\" standalone=\"yes\" encoding=\"UTF-8\" ?>\n<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n<html>\n<head>\n<meta http-equiv=\"content-type\" content=\"application/xhtml+xml;charset=utf-8\"/>\n<title>IM History with buddy "))
    {
        return;
    }

    // Import the chat log.
    QString $log_proc = $log_raw.toString();

    // Remove cruft that will interfere with QXmlStreamReader.
    QStringList $log_split = $log_proc.split("</head>");
    $log_split.pop_front();
    $log_proc = $log_split.join("</head>");
    $log_proc.replace("</body></html>", "</body>");

    // Get _self and _with
    QString $self, $with;
    // Non-group chat (_self and _with defined)
    if ($log_proc.contains("<a href=\"aim:goim?screenName="))
    {
        QString $users_proc = $log_proc;
        QStringList $users_split = $users_proc.split("<a href=\"aim:goim?screenName=");
        $users_split.pop_front();
        $users_proc = $users_split.join("<a href=\"aim:goim?screenName=");
        $users_split = $users_proc.split("&targetBuddyList=");
        $with = $users_split.takeFirst();
        $users_proc = $users_split.join("&targetBuddyList");
        $users_split = $users_proc.split("\">");
        $self = $users_split.takeFirst();
    }
    // Group chat (sorry; there's no _with information)
    else
    {
        $self = accountGuess;
        $with = buddyGuess;
    }

    // Create HTML reader
    QXmlStreamReader xml($log_raw.toString());

    // $log_raw is AIM chat log. Create new entry.
    final->newEntry();

    // TODO
}

/**
 * Generate Log from Standardized Log
 */
QVariant Aim::generate(StdFormat *$log)
{
    // TODO
}

/**
 * Process "From" Request
 */
StdFormat* Aim::from(QHash<QString, QVariant> data)
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

        /// "%PATHUPTOAIMLOGGER%/AIMLogger/your_username/IM Logs/buddy_username.html"
        QString unames_proc = i.key();

        // Guess the account and buddy (not accurate)
        accountGuess.clear();
        buddyGuess.clear();
        if (unames_proc.endsWith(".html"))
        {
            unames_proc = QDir::fromNativeSeparators(unames_proc);
            unames_proc.remove(unames_proc.length() - 4);
            QStringList unames_split = unames_proc.split("/");
            // Buddy Username
            buddyGuess = unames_split.takeLast();
            buddyGuess.remove(buddyGuess.length() - 10);
            if (buddyGuess.startsWith("conf-"))
                buddyGuess = "_group";
            // Cruft
            unames_split.pop_back();
            // Account Username
            accountGuess = unames_split.takeLast();
            accountGuess.remove(accountGuess.length() - 10);

            this->load($raw_item);
        }

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
void Aim::to(StdFormat* $log)
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
