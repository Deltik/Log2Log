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
    QString $log_refined; // whitespace fixed and trimmed version of $log_raw
    QString $log_date; // temporarily stores the date of a chatlog
    QStringList $log_chats; // contains all the chatlogs to be processed
    QByteArray *$log_date_bytes = new QByteArray();
    QList<QDateTime> $times;
    QDateTime $time_cur;
    QStringList $log_chat_entries;
    QStringList $log_entry_items;
    QString $from_sep = "<span class='ImReceive'>"; // from separator
    QString $to_sep = "<span class='ImSend'>"; // to separator
    QString $sender;
    bool $from;
    bool $to;
    qint8 $accuracy, $specificity;
    qint32 $count;
    //QString $log_chat : contains a a chat log to be processed (usually inside foreachs)

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

    // If there are no Meebo Chat Logs
    if($log_chats.size() == 0)
        return;

    // Retrieve Meebo log entries' start times
    foreach(QString $log_chat, $log_chats) {

        $log_date.clear();
        $log_date_bytes->clear();
        $log_date = $log_chat.split("</div><hr size=1>").first();

        // HTML entity decode
        $log_date = QUrl::fromPercentEncoding($log_date_bytes->append($log_date.toUtf8()));

        // TODO: standardize $log_date to time
        $times << QDateTime::fromString($log_date, "l, yyyy MMMM dd (HH:mm:ss)");
    }

    // Go through all the Meebo chatlogs
    foreach(QString $log_chat, $log_chats) {

        $time_cur = $times.first();

        // TODO: detection
        final->newEntry();
        final->setProtocol("meebo");
        final->setSelf("myself");
        final->setSelfAlias("myself");
        final->setWith("someone");
        final->setWithAlias("someone");
        final->setTime($time_cur.toString().toLongLong());

        $log_chat_entries = $log_chat.split("<br/>");

        // invalid entries
        $log_chat_entries.removeFirst();
        $log_chat_entries.removeLast();

        $count = 0;
        foreach(QString $log_chat_entry, $log_chat_entries) {

            // check if an entry is from or to the user
            $from = $to = false;
            if($log_chat_entry.indexOf($from_sep,0) == 0) {
                $from = true;
                $log_chat_entry = $log_chat_entry.mid($from_sep.length());
            }
            else if($log_chat_entry.indexOf($to_sep,0) == 0) {
                $to = true;
                $log_chat_entry = $log_chat_entry.mid($from_sep.length());
            }

            QRegExp re("/(\\[)((?:(?:[0-1][0-9])|(?:[2][0-3])|(?:[0-9])):(?:[0-5][0-9])(?::[0-5][0-9])?(?:\\s?(?:am|AM|pm|PM))?)(\\])/is");
            re.indexIn($log_chat_entry); // the index returned is irrelevant

            // If entry contains valid Meebo chat log timestamp...
            if(re.captureCount() == 5) { // 1 for the whole + 4 for each captured
                $time_cur = QDateTime::fromString($time_cur.toString("dd-MMMM-YYYY"),"dd-MMMM-YYYY amAM");

                // Set Log2Log Timestamp Spezcificity Index
                $specificity = 2;
                // Set Log2Log Message Content Accuracy Index
                $accuracy = 0;

                // The first Meebo chat log entry has a Log2Log Timestamp Specificity Index of 0.
                // It is the same value as the header time.
                if ($count == 0)
                {
                    $time_cur = $times.first();
                    $specificity = 0;
                }
            }

            // Get the sender name and get the entry message.
            $log_entry_items = $log_chat_entry.split("</span>: ");
            $log_entry_items.removeFirst();
            $log_chat_entry = $log_entry_items.join("</span>: )");

            // Clean up the sender's name.
            $sender = $log_entry_items.first().replace(0, re.capturedTexts().takeAt(1).length(), "").trimmed();

            // TODO: Clean up the entry message.
            $log_chat_entry.replace("<br>", "\n", Qt::CaseInsensitive);
            //$log_chat_entry = /*htmlspecialchars_decode(html_entity_decode(*/$log_chat_entry/*))*/;
            $log_chat_entry.replace("&apos;", "'", Qt::CaseInsensitive);

            // FINAL CONSTRUCTION
            final->newRow();
            final->setCode(0);
            final->setTime($time_cur.toString().toLongLong());
            if ($from)
            {
                final->setSender($sender);
                final->setAlias($sender); //TODO
            }
            else if ($to)
            {
                final->setSender("myself"); //TODO
                final->setAlias("someone"); //TODO
            }
            else
            {
                final->setSender("_unknown"); //TODO
                final->setAlias("_unknown"); //TODO
            }
            final->setContent($log_chat_entry);
            final->setPrecision($specificity); //TODO IS THIS ONE?
            final->setAccuracy($accuracy);
            final->setWithAlias("with_alias"); //TODO

            $count++;
        }
        $times.removeFirst();
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
