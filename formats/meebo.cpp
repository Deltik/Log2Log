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
    QStringList $log_entries; // contains all the chatlogs to be processed
    QByteArray *$log_date_bytes = new QByteArray();
    QList<QDateTime> $times;
    QDateTime $time_cur;
    QStringList $log_chatrows;
    //QStringList $log_entry_items;
    QString $chat_sep = "<br/><hr size=1><div class='ImChatHeader'>"; // chat instance separator
    QString $rece_sep = "<span class='ImReceive'>"; // from separator
    QString $send_sep = "<span class='ImSend'>"; // to separator
    QString $sender;
    QString $message;
    QTextEdit $htmldecoder; // workaround
    bool $rece, $send;
    bool $self_set, $with_set;
    qint8 $accuracy, $specificity;
    qint32 $count;
    //QString $log_entry : contains a chat log to be processed (inside foreach)
    // QString $log_chatrow : contains a chat log entry to be processed (inside foreach)

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
    $log_entries = $log_refined.split($chat_sep);
    $log_entries.removeFirst();

    // If there are no Meebo Chat Logs
    if($log_entries.size() == 0)
        return;

    // Retrieve Meebo log entries' start times
    foreach(QString $log_entry, $log_entries) {

        $log_date.clear();
        $log_date_bytes->clear();
        $log_date = $log_entry.split("</div><hr size=1>").first();

        // HTML entity decode
        $htmldecoder.setHtml($log_date);
        $log_date = $htmldecoder.toPlainText();

        // Remove the week day to ease conversion
        $log_date.replace(0,($log_date.indexOf(" ")+1),"");

        // Standardize (convert) $log_date to QDateTime
        $times << QDateTime::fromString($log_date, "yyyy MMMM dd (HH:mm:ss)");
    }

    // Go through all the Meebo chatlogs
    foreach(QString $log_entry, $log_entries) {

        $self_set = $with_set = false;
        $time_cur = $times.first();

        // TODO: detection
        final->newEntry();
        final->setProtocol($protocol);
        qDebug()  << "PROTO: " << final->getProtocol();
        final->setSelf($account);
        qDebug()  << "SELF: " << final->getSelf();
        final->setSelfAlias($account);
        final->setWith($with);
        qDebug()  << "WITH: " << final->getWith();
        final->setWithAlias($with);
        final->setTime($time_cur.toMSecsSinceEpoch());

        $log_chatrows = $log_entry.split("<br/>\n");

        // invalid entries
        $log_chatrows.removeFirst();
        $log_chatrows.removeLast();

        $count = 0;
        foreach(QString $log_chatrow, $log_chatrows) {

            $log_chatrow.replace("\n","");

            // check if an entry is from or to the user
            $rece = $send = false;
            if($log_chatrow.indexOf($rece_sep,0) == 0) {
                $rece = true;
                $log_chatrow = $log_chatrow.mid($rece_sep.length());
            }
            else if($log_chatrow.indexOf($send_sep,0) == 0) {
                $send = true;
                $log_chatrow = $log_chatrow.mid($send_sep.length());
            }

            QRegExp re("^\\[(([0-1][0-9]|[2][0-3]):([0-5][0-9]))\\] ([\\S ]+)</span>: ([\\S ]*)$");
            if(re.indexIn($log_chatrow) < 0)
                return;

            // If entry contains valid Meebo chat log timestamp...
            if(re.captureCount() != 5) // consult the documentation at the end of the file
                return;

            // Set the time of the current entry
            $time_cur.setTime(QTime::fromString(re.capturedTexts().takeAt(1),"HH:mm"));

            // Set Log2Log Timestamp Specificity Index
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

//            // Get the sender name and get the entry message.
//            $log_entry_items = $log_chatrow.split("</span>: ");
//            //$log_entry_items.removeFirst(); // seems to not be required
//            $log_chatrow = $log_entry_items.join("</span>: )");

//            // Clean up the sender's name.
//            $sender = $log_entry_items.first().replace(0, re.capturedTexts().takeAt(4).length(), "").trimmed();

            // TODO: Clean up the sender's name.
            $sender = re.capturedTexts().takeAt(4);

            // TODO: Clean up the entry message.
            $message = re.capturedTexts().takeAt(5);

//            // TODO: Clean up the entry message.
//            $log_chatrow.replace("<br>", "\n");
//            //$log_chatrow = /*htmlspecialchars_decode(html_entity_decode(*/$log_chatrow/*))*/;
//            $log_chatrow.replace("&apos;", "'");

            // FINAL CONSTRUCTION
            final->newRow();
            final->setCode(0);
            final->setTime($time_cur.toMSecsSinceEpoch());

            // TODO: the unknown person of the entry
            final->setSender($sender);
            qDebug()  << "SENDER: " << final->getSender();
            final->setAlias($sender);

            final->setContent($message);
            final->setPrecision($specificity);
            final->setAccuracy($accuracy);
            final->setWithAlias("with_alias");

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
        QString $content = "<br/><hr size=1><div class='ImChatHeader'>";

        // Put the longer variables into something more readily accessible.
        QString $protocol      = $log->getProtocol();
        QString $account       = $log->getSelf();
        QString $self_alias    = $log->getSelfAlias();
        QString $with          = $log->getWith();
        QString $with_alias    = $log->getWithAlias();
        qlonglong $time_base   = $log->getTime();
        QString $timezone_base = $log->getTimezone();
    }
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

/**

  A little documentation

  Each chat log entry is composed of 5 parts of data
  that are captured through a regular expression:

  0: the whole block of data (isn't counted for the capture count)
  1: time   (HH:mm)
  2: hour   (HH)
  3: minute (mm)
  4: sender
  5: message

 */
