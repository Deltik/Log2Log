/**
 * Log2Log Chat Log Converter
 *  Formats
 *   Standard Format
 *
 * @author Deltik
 * @brief Log2Log Generic Chat Log Structure: At bottom
 * @remarks Qt/C++ arrays (QHash and QMap) are ridiculously more complicated
 *          than arrays in PHP. To improve efficiency and avoid crazy
 *          multidimensional arrays, each type of data must be "extracted" from
 *          its QVariant. For example:
 *
 *          You have final["data"], which is a QMap inside a QVariant of final.
 *          If you want to insert what is logically final["data"][0], you must
 *          have the following code:
 *
 *          // 1. EXTRACT the data
 *          QMap extractedData = final.value("data").toMap();
 *          // 2. MODIFY the data
 *          extractedData.insert("0", "Something because this is a QVariant");
 *          // 3. REINSERT the data
 *          final["data"].setValue(extractedData);
 *
 *          or equally compatible code:
 *
 *          // 1. EXTRACT the data
 *          QMap extractedData = final["data"].toMap();
 *          // 2. MODIFY the data
 *          extractedData["0"] = "Something because this is a QVariant";
 *          // 3. REINSERT the data
 *          final["data"] = extractedData;
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

#include "stdformat.h"

/*DEBUG*/
#include <QDebug>

/**
 * Constructor
 */
StdFormat::StdFormat()
{
    // Version 1.2! Whoo! :D
    log["version"] = "1.2";

    // Give an empty client string to final["client"]
    log["client"] = "";
}


/**
 * Destructor
 */
StdFormat::~StdFormat()
{
    QMap<QString, QVariant> empty;
    log = empty;
}

/**
 * Saver: Apply log["data"]
 */
void StdFormat::applyData()
{
    applyEntry();
    applySystem();

    if (!data.isEmpty())
    {
        log["data"] = data;
        data = emptyl;
    }
}

/**
 * Saver: Apply log["data"][entry]
 */
void StdFormat::applyEntry()
{
    applyChat();

    if (!entry.isEmpty())
    {
        while (dexEntry > data.size() - 1)
            data << false;
        data[dexEntry] = entry;
        entry = empty;
    }
}

/**
 * Saver: Apply log["data"][entry]["chat"]
 */
void StdFormat::applyChat()
{
    applyRow();

    if (!chat.isEmpty())
    {
        entry["chat"] = chat;
        chat = emptyl;
    }
}

/**
 * Saver: Apply log["data"][entry]["chat"][row]
 */
void StdFormat::applyRow()
{
    if (!row.isEmpty())
    {
        while (dexRow > chat.size() - 1)
            chat << false;
        chat[dexRow] = row;
        row = empty;
    }
}

/**
 * Saver: Apply log["system"][row]
 */
void StdFormat::applySystem()
{
    if (!system.isEmpty())
    {
        QList<QVariant> systemHolder = log["system"].toList();
        while (dexSystem > systemHolder.size() - 1)
            systemHolder << false;
        systemHolder[dexSystem] = system;
        log["system"] = systemHolder;
        system = emptyl;
    }
}

/**
 * Saver: Ultimate Save
 */
void StdFormat::applyAuto()
{
    applyRow();
    applyChat();
    applyEntry();
    applySystem();
    applyData();
    // Now, all unsaved data is stored into the QMap log.
}

/**
 * Extractor: Extract log["data"]
 */
void StdFormat::extractData()
{
    if (data.isEmpty())
        data = log["data"].toList();
}

/**
 * Extractor: Extract log["data"][entry]
 */
void StdFormat::extractEntry()
{
    extractData();
    if (entry.isEmpty())
        entry = data[dexEntry].toMap();
}

/**
 * Extractor: Extract log["data"][entry]["chat"]
 */
void StdFormat::extractChat()
{
    extractEntry();
    if (chat.isEmpty())
        chat = entry["chat"].toList();
}

/**
 * Extractor: Extract log["data"][entry]["chat"][row]
 */
void StdFormat::extractRow()
{
    extractChat();
    if (row.isEmpty())
        row = chat[dexRow].toMap();
}

/**
 * Getter: Get Log2Log Generic Chat Log Format Version
 * @returns QString The version numberr
 */
QString StdFormat::getVersion()
{
    // Get the version number. Easy.
    return log["version"].toString();
}

/**
 * Setter: Set Client
 * @param QString name The name of the converting-from client
 * @returns bool Whether the client setting was successful
 */
bool StdFormat::setClient(QString name)
{
    // Set the name. Easy.
    log["client"] = name;
    // I guarantee a success.
    return true;
}

/**
 * Getter: Get Client
 * @returns QString The client name
 */
QString StdFormat::getClient()
{
    // Get the client name. Easy.
    return log["client"].toString();
}

/**
 * Browser: Reset Pointer/Browser
 * @returns void
 */
void StdFormat::resetPointer()
{
    // Flush working environment
    applyAuto();

    dexEntry  = -1;
    dexRow    = -1;
    inRow     = false;
    dexSystem = -1;
}

/**
 * Browser: New Entry
 * @returns int The entry number/index
 */
int StdFormat::newEntry()
{
    // Save then clear current entry
    applyEntry();
    // Move to next open space
    dexEntry = data.size();

    // Not pointing in a chat row
    inRow = false;

    // Return the new entry number
    return dexEntry;
}

/**
 * Browser: Does the Next Entry Exist?
 * @returns bool true if there is an entry after this one
 */
bool StdFormat::hasNextEntry()
{
    // Extract
    extractData();

    if (dexEntry >= data.size() - 1)
        return false;
    return true;
}

/**
 * Browser: Next Entry
 * @returns bool false if could not point to next entry
 */
bool StdFormat::nextEntry()
{
    // Extract
    extractData();

    if (dexEntry >= data.size() - 1)
        return false;
    dexEntry ++; dexRow = -1; inRow = false;

    // Set entry
    entry = empty;
    chat = emptyl;
    row = empty;
    extractEntry();

    return true;
}

/**
 * Browser: Does the Previous Entry Exist?
 * @returns bool true if there is an entry before this one
 */
bool StdFormat::hasPreviousEntry()
{
    if (dexEntry < 0)
        return false;
    return true;
}

/**
 * Browser: Previous Entry
 * @returns bool false if could not point to previous entry
 */
bool StdFormat::previousEntry()
{
    // Extract
    extractData();

    if (dexEntry < 0)
        return false;
    dexEntry --; dexRow = -1; inRow = false;

    // Set entry
    entry = empty;
    chat = emptyl;
    row = empty;
    extractEntry();

    return true;
}

/**
 * Browser: Jump to an Entry Index
 * @returns bool false if could not point to specified entry
 */
bool StdFormat::gotoEntry(int index)
{
    // Extract
    extractData();

    if (data[index].isNull())
        return false;
    dexEntry = index; dexRow = -1; inRow = false;

    // Set entry
    entry = empty;
    chat = emptyl;
    row = empty;
    extractEntry();

    return true;
}

/**
 * Browser: Switch To/From a Row From/To an Entry
 * @returns bool true if pointing in the row
 */
bool StdFormat::toggleRow()
{
    // Extract
    extractChat();

    // If already in a row, exit
    if (inRow)
    {
        inRow = false;
        return false;
    }

    // Check to see if final["data"][entry][row] is safe to enter.
    if (chat.isEmpty())
        return false;

    // Safe to enter. Enter.
    inRow = true;
    return true;
}

/**
 * Browser: New Chat Row
 * @returns int The row number/index
 */
int StdFormat::newRow()
{
    // Save then clear current chat row
    applyRow();
    // Move to next open space
    int count = chat.size();
    dexRow = count;

    // Now pointing in chat rows
    inRow = true;

    // Return current chat row number
    return dexRow;
}

/**
 * Browser: Does the Next Chat Row Exist?
 * @returns bool true if there is a chat row after this one
 */
bool StdFormat::hasNextRow()
{
    // Extract
    extractChat();

    if (dexRow >= chat.size() - 1)
        return false;

    // Now pointing in chat rows
    inRow = true;

    return true;
}

/**
 * Browser: Next Chat Row
 * @returns bool false if could not point to next chat row/index
 */
bool StdFormat::nextRow()
{
    // Extract
    extractChat();

    if (dexRow >= chat.size() - 1)
        return false;

    dexRow ++;

    // Now pointing in chat rows
    inRow = true;

    // Set chat row
    row = empty;
    extractRow();

    return true;
}

/**
 * Browser: Does the Previous Chat Row Exist?
 * @returns bool true if there is a chat row before this one
 */
bool StdFormat::hasPreviousRow()
{
    if (dexRow < 0)
        return false;

    // Now pointing in chat rows
    inRow = true;

    return true;
}

/**
 * Browser: Previous Chat Row
 * @returns bool false if could not point to previous chat row/index
 */
bool StdFormat::previousRow()
{
    // Extract
    extractChat();

    if (dexRow < 0)
        return false;

    dexRow --;

    // Now pointing in chat rows
    inRow = true;

    // Set chat row
    row = empty;
    extractRow();

    return true;
}

/**
 * Browser: Jump to a Chat Row/Index
 * @returns bool false if could not point to specified chat row/index
 */
bool StdFormat::gotoRow(int index)
{
    // Extract
    extractChat();

    // If selected chat row doesn't exist...
    if (chat[index].isNull())
        return false;

    dexRow = index;

    // Now pointing in chat rows
    inRow = true;

    // Set chat row
    row = empty;
    extractRow();

    return true;
}

/**
 * Setter: Set Protocol
 * @param QString name The protocol name to set
 * @returns bool true on success
 */
bool StdFormat::setProtocol(QString name)
{
    entry["protocol"] = name;

    return true;
}

/**
 * Setter: Set Self
 * @param QString username The self account username to set
 * @returns bool true on success
 */
bool StdFormat::setSelf(QString username)
{
    entry["self"] = username;

    return true;
}

/**
 * Setter: Set Self Alias
 * @param QString myname The self account display name to set
 * @returns bool true on success
 */
bool StdFormat::setSelfAlias(QString myname)
{
    entry["self_alias"] = myname;

    return true;
}

/**
 * Setter: Set With
 * @param QString username The primary buddy account username to set
 * @returns bool true on success
 */
bool StdFormat::setWith(QString username)
{
    entry["with"] = username;

    return true;
}

/**
 * Setter: Set With Alias
 * @param QString username The primary buddy account display name to set
 * @returns bool true on success
 */
bool StdFormat::setWithAlias(QString buddyname)
{
    entry["with_alias"] = buddyname;

    return true;
}

/**
 * Setter: Set Time
 * @param qint64 time UNIX epoch of the time to set
 * @returns bool true on success
 */
bool StdFormat::setTime(qlonglong time)
{
    // Default (entry) timezone or chat row timezone? inRow decides.
    if (!inRow)
    {
        entry["time"] = time;
    }
    else
    {
        row["time"] = time;
    }

    return true;
}

/**
 * Setter: Set Timezone
 * @param QString timezone Timezone identifier to accompany the UNIX time
 * @returns bool true on success
 */
bool StdFormat::setTimezone(QString timezone)
{
    // Default (entry) timezone or chat row timezone? inRow decides.
    if (!inRow)
    {
        entry["timezone"] = timezone;
    }
    else
    {
        row["timezone"] = timezone;
    }

    return true;
}

/**
 * Setter: Set Sender
 * @param QString name Username of the sender
 * @returns bool true on success
 */
bool StdFormat::setSender(QString name)
{
    row["sender"] = name;

    return true;
}

/**
 * Setter: Set Sender Alias
 * @param QString name Alias of the sender
 * @returns bool true on success
 */
bool StdFormat::setAlias(QString name)
{
    row["alias"] = name;

    return true;
}

/**
 * Setter: Set Message Content
 * @param QString content Content of the message
 * @returns bool true on success
 */
bool StdFormat::setContent(QString content)
{
    row["content"] = content;

    return true;
}

/**
 * Setter: Set Log2Log Message Code
 * @param int code Code ID
 * @returns bool true on success
 */
bool StdFormat::setCode(int code)
{
    row["code"] = code;

    return true;
}

/**
 * Setter: Set Log2Log Timestamp Precision Index
 * @param int precision Log2Log Timestamp Precision Index
 * @returns bool true on success
 */
bool StdFormat::setPrecision(int precision)
{
    row["precision"] = precision;

    return true;
}

/**
 * Setter: Set Log2Log Message Content Accuracy Index
 * @param int accuracy Log2Log Message Content Accuracy Index
 * @returns bool true on success
 */
bool StdFormat::setAccuracy(int accuracy)
{
    row["accuracy"] = accuracy;

    return true;
}

/**
 * Setter: Set Log2Log Message Niceness
 * @param int nice Log2Log message nice value
 * @returns bool true on success
 */
bool StdFormat::setNice(int nice)
{
    row["nice"] = nice;

    return true;
}

/**
 * Getter: Get Protocol
 * @returns QString The protocol name
 */
QString StdFormat::getProtocol()
{
    // Extract
    extractEntry();

    return entry["protocol"].toString();
}

/**
 * Getter: Get Self
 * @returns QString The account name of the primary person
 */
QString StdFormat::getSelf()
{
    // Extract
    extractEntry();

    return entry["self"].toString();
}

/**
 * Getter: Get Self Alias
 * @returns QString The self account display name to get
 */
QString StdFormat::getSelfAlias()
{
    // Extract
    extractEntry();

    return entry["self_alias"].toString();
}

/**
 * Getter: Get With
 * @returns QString The primary buddy account username to get
 */
QString StdFormat::getWith()
{
    // Extract
    extractEntry();

    return entry["with"].toString();
}

/**
 * Getter: Get With Alias
 * @returns QString The primary buddy account display name to get
 */
QString StdFormat::getWithAlias()
{
    // Extract
    extractEntry();

    return entry["with_alias"].toString();
}

/**
 * Getter: Get Time
 * @returns int UNIX epoch of the time to get
 */
qlonglong StdFormat::getTime()
{
    if (!inRow)
    {
        // Extract
        extractEntry();

        return entry["time"].toLongLong();
    }
    // Extract
    extractRow();

    return row["time"].toLongLong();
}

/**
 * Getter: Get Timezone
 * @returns QString Timezone identifier to accompany the UNIX time
 */
QString StdFormat::getTimezone()
{
    if (!inRow)
    {
        // Extract
        extractEntry();

        return entry["timezone"].toString();
    }
    // Extract
    extractRow();

    return row["timezone"].toString();
}

/**
 * Getter: Get Sender
 * @returns QString Username of the sender
 */
QString StdFormat::getSender()
{
    // Extract
    extractRow();

    return row["sender"].toString();
}

/**
 * Getter: Get Sender Alias
 * @returns QString Alias of the sender
 */
QString StdFormat::getAlias()
{
    // Extract
    extractRow();

    return row["alias"].toString();
}

/**
 * Getter: Get Message Content
 * @returns QString Content of the message
 */
QString StdFormat::getContent()
{
    // Extract
    extractRow();

    return row["content"].toString();
}

/**
 * Getter: Get Log2Log Message Code
 * @returns int Code ID
 */
int StdFormat::getCode()
{
    // Extract
    extractRow();

    return row["code"].toInt();
}

/**
 * Getter: Get Log2Log Timestamp Precision Index
 * @returns int Log2Log Timestamp Precision Index
 */
int StdFormat::getPrecision()
{
    // Extract
    extractRow();

    return row["precision"].toInt();
}

/**
 * Getter: Set Log2Log Message Content Accuracy Index
 * @returns int Log2Log Message Content Accuracy Index
 */
int StdFormat::getAccuracy()
{
    // Extract
    extractRow();

    return row["accuracy"].toInt();
}

/**
 * Getter: Get Log2Log Message Niceness
 * @returns int Log2Log message nice value
 */
int StdFormat::getNice()
{
    // Extract
    extractRow();

    return row["nice"].toInt();
}

/*

Log2Log Generic Chat Log Structure
TODO: Redraw the structure to make it look better.

"version" -> Log2Log Generic Chat Log Structure version
"client" -> converting-from client name; ex. Meebo, Adium, Pidgin
"data" -> actual log information
    (int) -> array of log entry, usually a file in most clients
        "protocol" -> service as recognized by Log2Log; ex. Facebook Chat, Google Talk
        "self" -> username of the primary member; ex. "me@gmx.com"
        "self_alias" -> alias of the primary member of the chat; ex. "Me"
        "with" -> the primary other member of the chat; ex. "you@gmx.com"
        "with_alias" -> alias of the primary other member of a chat; ex. "Best Friend"
        (int)"time" -> the start UNIX epoch of the chat; ex. 1321009871
        "timezone" -> the start time's timezone; ex. America/Chicago
        (array)"chat" -> the meat, A.K.A. the conversation
            (int)"time" -> the UNIX epoch of the entry; ex. 1321009871
            (int)"timezone" -> timezone identifier for the given time; ex. America/Chicago
            "sender" -> who sent the message; "_with" defaults to "with"; ex. "you@gmx.com"
            "alias" -> a more readable "sender"; "_with" defaults to "with_alias", if applicable
            "content" -> what the sender actually sent; ex. "I found a five-dollar bill!"
            (int)"code" -> identifier for what type of entry this is
            (int)"precision" -> an index of how accurate the time is; ex. 2
            (int)"accuracy" -> how accurate the "content" is as an index; ex. 3
            (int)"nice" -> how urgent this entry is from -20 (most) to 20 (least) (not implemented); ex. 3
        "system" -> system log; TODO: TBD
            (int) -> array of system log entry; TODO: TBD

Undescore Codes:

_self           You
_with           The primary other member of the chat
_with_alias     What the primary other member is known as
_unknown        An untitled member of the conversation
_evt_offline    Went offline
_evt_online     Went online
_evt_away       Went away
_evt_idle       Turned idle
_evt_close      Closed conversation
_evt_open       Opened conversation
_evt_fail       Message failed to send
_group_join     Someone joined the group
_group_leave    Someone left the group


Codes:

0   Normal message
1   Event
2   Unknown


Protocols:

aim         AIM
bonjour     Bonjour
facebook    Facebook
jabber      XMPP
icq         ICQ
irc         IRC
msn         MSN
skype       Skype
yahoo       YIM

Log2Log Timestamp Precision Index
 Legend:
  null: No time information
  -3: Nanoseconds
  -2: Microseconds
  -1: Milliseconds
  0 : Seconds
  1 : Ten Seconds
  2 : Minutes
  3 : Ten Minutes
  4 : Hours
  5 : Ten Hours
  6 : Day
  7 : Within a Week of a Date
  8 : Within a Month of a Date
  9 : Within a Year of a Date
  10: Within a Decade of a Date
  11: Within a Century of a Date
  12: Within a Millenium of a Date


Log2Log Message Content Accuracy Index
 Legend:
  0 : Exactly
  1 : Approximately/Very Similar To
  2 : Estimated/Similar To
  3 : Extrapolated/Guessed
  4 : Unconfirmed/Dramatized


Log2Log Message Niceness
 Legend:
  -20 : Eternally Urgent
  0   : Normal
  20  : Not Important at All

*/
