/**
 * Log2Log
 *  Formats
 *   Meebo HTML (AKA MeeboFarewell)
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

#include "meebofarewell.h"
#include "helper.h"
#include <QtXml>
#include <QDateTime>

/**
 * Constructor
 */
MeeboFarewell::MeeboFarewell()
{
    final = new StdFormat();
    final->setClient("Meebo");
}

/**
 * Load a Chat Log
 */
void MeeboFarewell::load(QVariant $log_raw, QString protocol)
{
    // Copyandpaste Copyandpaste Copyandpaste
    // I've given up on this project branch, so I'm just ripping the code right out of class Pidgin and class Meebo.
    QString thisSoftwareIsStupid = "<!doctype html><meta charset=\"utf-8\"/><style>html{font-family:Tahoma,Arial,sans-serif;font-size:11px;background:white;color:black;}h1{font-size:16px;}.ImSend{color:#D35900;}.ImReceive{color:#0163B3;}</style><h1>Chatlog between <span class=\"sender\">";
    if ($log_raw.toString().left(thisSoftwareIsStupid.length()) == thisSoftwareIsStupid)
        loadHtml($log_raw, protocol);
    else
        qDebug() << "Please die. :(";
}

/**
 * Load an HTML Meebo Chat Log
 */
void MeeboFarewell::loadHtml(QVariant $log_raw, QString protocol)
{
    // IDKWHAT STUFF
    QString $log_refined; // whitespace fixed and trimmed version of $log_raw
    QString $log_date; // temporarily stores the date of a chatlog
    QStringList $log_entries; // contains all the chatlogs to be processed
    QDateTime $time_cur;
    QStringList $log_chatrows;
    QString $row_regex = "^\\[(([0-1][0-9]|[2][0-3]):([0-5][0-9]))\\] ([\\S ]+)</span>: ([\\S ]*)$";
    QString $date_sep = "</div><hr size=1>"; // entry date separator
    QString $chat_sep = "<br/><hr size=1><div class='ImChatHeader'>"; // chat instance separator
    QString $rece_sep = "<span class='ImReceive'>"; // from separator
    QString $send_sep = "<span class='ImSend'>"; // to separator
    QString $sender;
    QString $message;
    QTextDocument $htmldecoder; // workaround
    bool $receiving, $sending;
    bool $self_set, $with_set;
    qint8 $accuracy, $specificity;
    qint32 $count;

    // Import the chat log.
    QString $log_proc = $log_raw.toString();

    // Assume that the inputted log is in Meebo HTML. Create new entry.
    final->newEntry();

    // Remove cruft that will interfere with QXmlStreamReader.
    QStringList $log_split = $log_proc.split("</style>");
    $log_split.pop_front();
    $log_proc = $log_split.join("</style>");

    // Line 1 of $log_proc is guaranteed log meta.
    // It looks like this:
    //  - "<h1>Chatlog between <span class="sender">_SELF</span> and <span class="receiver">_WITH</span> on <span title="2010-08-09T12:34:56Z">_TIME_STRING_IRRELEVANT</span></h1>"
    QString $log_header_proc = $log_proc.mid(41);
    QStringList $log_header_split = $log_header_proc.split("</span> and <span class=\"receiver\">");
    // CONSTRUCT: _self
    final->setSelf($log_header_split.takeFirst());
    $log_header_proc = $log_header_split.join("</span> and <span class=\"receiver\">");
    $log_header_split = $log_header_proc.split("</span> on <span title=\"");
    // CONSTRUCT: _with
    final->setWith($log_header_split.takeFirst());
    $log_header_proc = $log_header_split.join("</span> on <span title=\"");
    $log_header_split = $log_header_proc.split("\">");
    QString $date_proc = $log_header_split.takeFirst();
    $log_header_proc = $log_header_split.join("\">");
    // Banking on Qt::ISODate
    QDateTime $time_proc = QDateTime::fromString($date_proc, Qt::ISODate);
    // CONSTRUCT: _time
    qlonglong               $time_base;
    $time_base = $time_proc.toMSecsSinceEpoch();
    final->setTime($time_base);
    // CONSTRUCT: _protocol
    final->setProtocol(protocol);

    // TODAY
    QDate TODAY = $time_proc.date();
    int ADDY = 0;
    QDateTime LAITY;
    LAITY.setTime(QTime($time_proc.toString("H").toInt(), $time_proc.toString("m").toInt()));

    // Get crackin'.

    // WORK BY LINE
    $log_header_split = $log_header_proc.split("\n");
    $log_header_split.takeFirst();
    $log_header_proc = $log_header_split.join("\n");
    $log_chatrows = $log_header_proc.split("<br/>\n");

    foreach (QString $log_chatrow, $log_chatrows)
    {
        $log_chatrow.replace("\n","");

        // Check if an entry is from or to the user
        $receiving = $sending = false;
        if($log_chatrow.indexOf($rece_sep,0) == 0) {
            $receiving = true;
            $log_chatrow = $log_chatrow.mid($rece_sep.length());
        }
        else if($log_chatrow.indexOf($send_sep,0) == 0) {
            $sending = true;
            $log_chatrow = $log_chatrow.mid($send_sep.length());
        }

        QRegExp re($row_regex);
        if(re.indexIn($log_chatrow) < 0)
            continue;

        // If entry contains valid Meebo chat log timestamp...
        if(re.captureCount() != 5) // consult the documentation at the end of the file
            continue;

        // Set the time of the current entry
        $time_cur.setTime(QTime::fromString(re.capturedTexts().takeAt(1),"HH:mm"));
        // Check for time travel
        if ($time_cur.toMSecsSinceEpoch() > LAITY.toMSecsSinceEpoch())
            ADDY ++;
        LAITY = $time_cur;

        $time_cur.setDate(TODAY.addDays(ADDY));

        // Set Log2Log Timestamp Specificity Index
        $specificity = 2;
        // Set Log2Log Message Content Accuracy Index
        $accuracy = 0;

        // The first Meebo chat log entry has a Log2Log Timestamp Specificity Index of 0.
        // It is the same value as the header time.
        if ($count == 0)
        {
            $time_cur = $time_proc;
            $specificity = 0;
        }

        $sender = re.capturedTexts().takeAt(4); // Sender alias
        $message = re.capturedTexts().takeAt(5); // Message content

        final->newRow();
        final->setCode(0);
        final->setTime($time_cur.toMSecsSinceEpoch());

        // Set row data
        if($receiving)
            final->setSender(final->getWith());
        else if($sending)
            final->setSender(final->getSelf());
        else
            final->setSender("_unknown");
        final->setAlias($sender);
        final->setContent($message);
        final->setPrecision($specificity);
        final->setAccuracy($accuracy);
        final->setWithAlias("with_alias");

        $count++;
    }
}

/**
 * Generate Log from Standardized Log
 * TODO: Finish
 */
QVariant MeeboFarewell::generate(StdFormat *$log)
{
    // He's dead, Jim!
}

/**
 * MEEBO_CUSTOM: Set Account
 */
void MeeboFarewell::setAccount(QString account)
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
void MeeboFarewell::setProtocol(QString protocol)
{
    $protocol = protocol;
}

/**
 * MEEBO_CUSTOM: Set Other User's Account
 */
void MeeboFarewell::setWith(QString with)
{
    $with = with;
}

/**
 * Process "From" Request
 */
StdFormat* MeeboFarewell::from(QHash<QString, QVariant> data)
{
    QString filepath;
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
        filepath = i.key();
        // Remove directory path
        QStringList dirs_parts = QDir::fromNativeSeparators(filepath).split("/");
        dirs_parts.takeLast();
        dirs_parts.takeLast();
        dirs_parts.takeLast();
        QString protocol = dirs_parts.takeLast();
        filepath = dirs_parts.takeLast();

        this->load($raw_item, protocol);
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
void MeeboFarewell::to(StdFormat* $log)
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
