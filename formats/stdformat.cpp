/**
 * Log2Log Online Chat Log Converter
 *  Formats
 *   Standard Format
 *
 * @author Deltik
 * @brief Log2Log Generic Chat Log Structure: At bottom
 * @remarks Qt/C++ arrays (QMap and QHash) are rediculously more complicated
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
 */

#include "stdformat.h"

/**
 * Constructor
 */
StdFormat::StdFormat()
{
    // Version 1.2! Whoo! :D
    final["version"] = "1.2";

    // Give an empty client string to final["client"]
    final["client"] = "";

    // Give an empty QMap to final["data"]
    QMap<QString, QVariant> data_empty;
    final.insert("data", data_empty);
}


/**
 * Destructor
 */
StdFormat::~StdFormat()
{
    QHash<QString, QVariant> empty;
    final = empty;
}

/**
 * Setter: Set Client
 * @param QString name The name of the converting-from client
 * @returns bool Whether the client setting was successful
 */
bool StdFormat::setClient(QString name)
{
    // Set the name. Easy.
    final["client"] = name;
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
    return final["client"].toString();
}

/**
 * Browser: New Entry
 * @returns int The entry number/index
 */
int StdFormat::newEntry()
{
    QString curEntry;
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    int count = data.size();
    dexEntry = count;
    curEntry.setNum(dexEntry);
    QHash<QString, QVariant> empty;
    QVariant entry_empty(empty);
    // Edit
    data.insert(curEntry, entry_empty);
    // Reinsert
    final["data"] = data;

    // Return current entry number
    return dexEntry;
}

/**
 * Browser: Next Entry
 * @returns bool false if could not point to next entry
 */
bool StdFormat::nextEntry()
{
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    if (data.size() - 1 >= dexEntry)
        return false;
    dexEntry ++;
    return true;
}

/**
 * Browser: Previous Entry
 * @returns bool false if could not point to previous entry
 */
bool StdFormat::previousEntry()
{
    if (dexEntry == 0)
        return false;
    dexEntry --;
    return true;
}

/**
 * Browser: Jump to an Entry Index
 * @returns bool false if could not point to specified entry
 */
bool StdFormat::gotoEntry(int index)
{
    QString indexStr;
    indexStr.setNum(index);
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    if (data[indexStr].isNull())
        return false;
    dexEntry = index;
    return true;
}

/**
 * Browser: Switch To/From a Line From/To an Entry
 * @returns bool true if pointing in the line
 */
bool StdFormat::toggleLine()
{
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    if (inLine)
    {
        inLine = false;
        return false;
    }
    // Check to see if final["data"][dexEntry]["chat"] is safe to enter.
    QString curEntry;
    curEntry.setNum(dexEntry);
    if (!data[curEntry].toMap().contains("chat"))
        return false;
    if (data[curEntry].toMap().value("chat").toMap().isEmpty())
        return false;
    // Safe to enter. Enter.
    inLine = true;
    return true;
}

/**
 * Browser: New Chat Row
 * @returns int The row number/index
 */
int StdFormat::newLine()
{
    QString curEntry;
    QString curLine;
    curEntry.setNum(dexEntry);
    curLine.setNum(dexLine);
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    QHash<QString, QVariant> entry = data[curEntry].toHash();
    // Create chat row if no rows exist
    if (entry.value("chat").isNull())
    {
        QMap<QString, QVariant> empty;
        dexLine = 0; curLine = "0";
        // Edit
        QHash<QString, QVariant> emptyHash;
        QVariant chatRow_empty(emptyHash);
        empty.insert(curLine, chatRow_empty);
        QVariant chat_empty(empty);
        entry.insert("chat", chat_empty);
    }
    else
    {
        QMap<QString, QVariant> chat = entry["chat"].toMap();
        int count = chat.size();
        dexLine = count;
        curLine.setNum(dexLine);
        // Edit
        QHash<QString, QVariant> empty;
        QVariant chatRow_empty(empty);
        chat.insert(curLine, chatRow_empty);
        entry["chat"] = chat;
    }
    // Now pointing in chat rows
    inLine = true;
    // Reinsert
    data[curEntry] = entry;
    final["data"] = data;

    // Return current entry number
    return dexLine;
}

/**
 * Browser: Next Chat Row
 * @returns bool false if could not point to next chat row/index
 */
bool StdFormat::nextLine()
{
    QString curEntry;
    curEntry.setNum(dexEntry);
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    QHash<QString, QVariant> entry = data[curEntry].toHash();
    // If chat rows don't exist in this entry...
    if (entry.value("chat").isNull())
        return false;
    QMap<QString, QVariant> chat = entry["chat"].toMap();
    if (chat.size() - 1 >= dexLine)
        return false;
    dexLine ++;
    // Now pointing in chat rows
    inLine = true;
    return true;
}

/**
 * Browser: Previous Chat Row
 * @returns bool false if could not point to previous chat row/index
 */
bool StdFormat::previousLine()
{
    if (dexLine == 0)
        return false;
    dexLine --;
    // Now pointing in chat rows
    inLine = true;
    return true;
}

/**
 * Browser: Jump to a Chat Row/Index
 * @returns bool false if could not point to specified chat row/index
 */
bool StdFormat::gotoLine(int index)
{
    QString curEntry;
    QString curLine;
    curEntry.setNum(dexEntry);
    curLine.setNum(dexLine);
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    QHash<QString, QVariant> entry = data[curEntry].toHash();
    // If chat rows don't exist in this entry...
    if (entry.value("chat").isNull())
        return false;
    QMap<QString, QVariant> chat = entry["chat"].toMap();
    // If selected chat row doesn't exist...
    if (chat[curLine].isNull())
        return false;
    dexLine = index;
    // Now pointing in chat rows
    inLine = true;
    return true;
}

/**
 * Setter: Set Protocol
 * @param QString name The protocol name to set
 * @returns bool true on success
 */
bool StdFormat::setProtocol(QString name)
{
    QString curEntry;
    curEntry.setNum(dexEntry);
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    // If selected entry doesn't exist...
    if (data[curEntry].isNull())
        return false;
    QHash<QString, QVariant> entry = data[curEntry].toHash();
    entry["protocol"] = name;
    // Reinsert
    data[curEntry] = entry;
    final["data"] = data;

    return true;
}

/**
 * Setter: Set Self
 * @param QString username The self account username to set
 * @returns bool true on success
 */
bool StdFormat::setSelf(QString username)
{
    QString curEntry;
    curEntry.setNum(dexEntry);
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    // If selected entry doesn't exist...
    if (data[curEntry].isNull())
        return false;
    QHash<QString, QVariant> entry = data[curEntry].toHash();
    entry["self"] = username;
    // Reinsert
    data[curEntry] = entry;
    final["data"] = data;

    return true;
}

/**
 * Setter: Set Self Alias
 * @param QString myname The self account display name to set
 * @returns bool true on success
 */
bool StdFormat::setSelfAlias(QString myname)
{
    QString curEntry;
    curEntry.setNum(dexEntry);
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    // If selected entry doesn't exist...
    if (data[curEntry].isNull())
        return false;
    QHash<QString, QVariant> entry = data[curEntry].toHash();
    entry["self_alias"] = myname;
    // Reinsert
    data[curEntry] = entry;
    final["data"] = data;

    return true;
}

/**
 * Setter: Set With
 * @param QString username The primary buddy account username to set
 * @returns bool true on success
 */
bool StdFormat::setWith(QString username)
{
    QString curEntry;
    curEntry.setNum(dexEntry);
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    // If selected entry doesn't exist...
    if (data[curEntry].isNull())
        return false;
    QHash<QString, QVariant> entry = data[curEntry].toHash();
    entry["with"] = username;
    // Reinsert
    data[curEntry] = entry;
    final["data"] = data;

    return true;
}

/**
 * Setter: Set With Alias
 * @param QString username The primary buddy account display name to set
 * @returns bool true on success
 */
bool StdFormat::setWithAlias(QString buddyname)
{
    QString curEntry;
    curEntry.setNum(dexEntry);
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    // If selected entry doesn't exist...
    if (data[curEntry].isNull())
        return false;
    QHash<QString, QVariant> entry = data[curEntry].toHash();
    entry["with_alias"] = buddyname;
    // Reinsert
    data[curEntry] = entry;
    final["data"] = data;

    return true;
}

/**
 * Setter: Set Time
 * @param qint64 time UNIX epoch of the time to set
 * @returns bool true on success
 */
bool StdFormat::setTime(qint64 time)
{
    QString curEntry;
    QString curLine;
    curEntry.setNum(dexEntry);
    curLine.setNum(dexLine);
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    // If selected entry doesn't exist...
    if (data[curEntry].isNull())
        return false;
    QHash<QString, QVariant> entry = data[curEntry].toHash();
    // Default (entry) timezone or chat row timezone? inLine decides.
    if (!inLine)
    {
        entry["time"] = time;
    }
    else
    {
        // If the entry hasn't initialized any chat rows...
        if (entry["chat"].isNull())
            return false;
        QMap<QString, QVariant> chat = entry["chat"].toMap();
        // If the set chat row doesn't exist...
        if (chat[curLine].isNull())
            return false;
        QHash<QString, QVariant> chatRow = chat[curLine].toHash();
        // Reinsert
        chatRow["time"] = time;
        chat[curLine] = chatRow;
        entry["chat"] = chat;
    }
    // Reinsert
    data[curEntry] = entry;
    final["data"] = data;

    return true;
}

/**
 * Setter: Set Timezone
 * @param QString timezone Timezone identifier to accompany the UNIX time
 * @returns bool true on success
 */
bool StdFormat::setTimezone(QString timezone)
{
    QString curEntry;
    QString curLine;
    curEntry.setNum(dexEntry);
    curLine.setNum(dexLine);
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    // If selected entry doesn't exist...
    if (data[curEntry].isNull())
        return false;
    QHash<QString, QVariant> entry = data[curEntry].toHash();
    // Default (entry) timezone or chat row timezone? inLine decides.
    if (!inLine)
    {
        entry["timezone"] = timezone;
    }
    else
    {
        // If the entry hasn't initialized any chat rows...
        if (entry["chat"].isNull())
            return false;
        QMap<QString, QVariant> chat = entry["chat"].toMap();
        // If the set chat row doesn't exist...
        if (chat[curLine].isNull())
            return false;
        QHash<QString, QVariant> chatRow = chat[curLine].toHash();
        // Reinsert
        chatRow["timezone"] = timezone;
        chat[curLine] = chatRow;
        entry["chat"] = chat;
    }
    // Reinsert
    data[curEntry] = entry;
    final["data"] = data;

    return true;
}

/**
 * Setter: Set Sender
 * @param QString name Username of the sender
 * @returns bool true on success
 */
bool StdFormat::setSender(QString name)
{
    QString curEntry;
    QString curLine;
    curEntry.setNum(dexEntry);
    curLine.setNum(dexLine);
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    // If selected entry doesn't exist...
    if (data[curEntry].isNull())
        return false;
    QHash<QString, QVariant> entry = data[curEntry].toHash();
    // If the entry hasn't initialized any chat rows...
    if (entry["chat"].isNull())
        return false;
    QMap<QString, QVariant> chat = entry["chat"].toMap();
    // If the set chat row doesn't exist...
    if (chat[curLine].isNull())
        return false;
    QHash<QString, QVariant> chatRow = chat[curLine].toHash();
    // Reinsert
    chatRow["sender"] = name;
    chat[curLine] = chatRow;
    entry["chat"] = chat;
    // Reinsert
    data[curEntry] = entry;
    final["data"] = data;

    return true;
}

/**
 * Setter: Set Sender Alias
 * @param QString name Alias of the sender
 * @returns bool true on success
 */
bool StdFormat::setAlias(QString name)
{
    QString curEntry;
    QString curLine;
    curEntry.setNum(dexEntry);
    curLine.setNum(dexLine);
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    // If selected entry doesn't exist...
    if (data[curEntry].isNull())
        return false;
    QHash<QString, QVariant> entry = data[curEntry].toHash();
    // If the entry hasn't initialized any chat rows...
    if (entry["chat"].isNull())
        return false;
    QMap<QString, QVariant> chat = entry["chat"].toMap();
    // If the set chat row doesn't exist...
    if (chat[curLine].isNull())
        return false;
    QHash<QString, QVariant> chatRow = chat[curLine].toHash();
    // Reinsert
    chatRow["alias"] = name;
    chat[curLine] = chatRow;
    entry["chat"] = chat;
    // Reinsert
    data[curEntry] = entry;
    final["data"] = data;

    return true;
}

/**
 * Setter: Set Message Content
 * @param QString content Content of the message
 * @returns bool true on success
 */
bool StdFormat::setContent(QString content)
{
    QString curEntry;
    QString curLine;
    curEntry.setNum(dexEntry);
    curLine.setNum(dexLine);
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    // If selected entry doesn't exist...
    if (data[curEntry].isNull())
        return false;
    QHash<QString, QVariant> entry = data[curEntry].toHash();
    // If the entry hasn't initialized any chat rows...
    if (entry["chat"].isNull())
        return false;
    QMap<QString, QVariant> chat = entry["chat"].toMap();
    // If the set chat row doesn't exist...
    if (chat[curLine].isNull())
        return false;
    QHash<QString, QVariant> chatRow = chat[curLine].toHash();
    // Reinsert
    chatRow["content"] = content;
    chat[curLine] = chatRow;
    entry["chat"] = chat;
    // Reinsert
    data[curEntry] = entry;
    final["data"] = data;

    return true;
}

/**
 * Setter: Set Log2Log Message Code
 * @param int code Code ID
 * @returns bool true on success
 */
bool StdFormat::setCode(int code)
{
    QString curEntry;
    QString curLine;
    curEntry.setNum(dexEntry);
    curLine.setNum(dexLine);
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    // If selected entry doesn't exist...
    if (data[curEntry].isNull())
        return false;
    QHash<QString, QVariant> entry = data[curEntry].toHash();
    // If the entry hasn't initialized any chat rows...
    if (entry["chat"].isNull())
        return false;
    QMap<QString, QVariant> chat = entry["chat"].toMap();
    // If the set chat row doesn't exist...
    if (chat[curLine].isNull())
        return false;
    QHash<QString, QVariant> chatRow = chat[curLine].toHash();
    // Reinsert
    chatRow["code"] = code;
    chat[curLine] = chatRow;
    entry["chat"] = chat;
    // Reinsert
    data[curEntry] = entry;
    final["data"] = data;

    return true;
}

/**
 * Setter: Set Log2Log Timestamp Specificity Index
 * @param int specificity Log2Log Timestamp Specificity Index
 * @returns bool true on success
 */
bool StdFormat::setSpecificity(int specificity)
{
    QString curEntry;
    QString curLine;
    curEntry.setNum(dexEntry);
    curLine.setNum(dexLine);
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    // If selected entry doesn't exist...
    if (data[curEntry].isNull())
        return false;
    QHash<QString, QVariant> entry = data[curEntry].toHash();
    // If the entry hasn't initialized any chat rows...
    if (entry["chat"].isNull())
        return false;
    QMap<QString, QVariant> chat = entry["chat"].toMap();
    // If the set chat row doesn't exist...
    if (chat[curLine].isNull())
        return false;
    QHash<QString, QVariant> chatRow = chat[curLine].toHash();
    // Reinsert
    chatRow["specificity"] = specificity;
    chat[curLine] = chatRow;
    entry["chat"] = chat;
    // Reinsert
    data[curEntry] = entry;
    final["data"] = data;

    return true;
}

/**
 * Setter: Set Log2Log Message Content Accuracy Index
 * @param int accuracy Log2Log Message Content Accuracy Index
 * @returns bool true on success
 */
bool StdFormat::setAccuracy(int accuracy)
{
    QString curEntry;
    QString curLine;
    curEntry.setNum(dexEntry);
    curLine.setNum(dexLine);
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    // If selected entry doesn't exist...
    if (data[curEntry].isNull())
        return false;
    QHash<QString, QVariant> entry = data[curEntry].toHash();
    // If the entry hasn't initialized any chat rows...
    if (entry["chat"].isNull())
        return false;
    QMap<QString, QVariant> chat = entry["chat"].toMap();
    // If the set chat row doesn't exist...
    if (chat[curLine].isNull())
        return false;
    QHash<QString, QVariant> chatRow = chat[curLine].toHash();
    // Reinsert
    chatRow["accuracy"] = accuracy;
    chat[curLine] = chatRow;
    entry["chat"] = chat;
    // Reinsert
    data[curEntry] = entry;
    final["data"] = data;

    return true;
}

/**
 * Setter: Set Log2Log Message Niceness
 * @param int nice Log2Log message nice value
 * @returns bool true on success
 */
bool StdFormat::setNice(int nice)
{
    QString curEntry;
    QString curLine;
    curEntry.setNum(dexEntry);
    curLine.setNum(dexLine);
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    // If selected entry doesn't exist...
    if (data[curEntry].isNull())
        return false;
    QHash<QString, QVariant> entry = data[curEntry].toHash();
    // If the entry hasn't initialized any chat rows...
    if (entry["chat"].isNull())
        return false;
    QMap<QString, QVariant> chat = entry["chat"].toMap();
    // If the set chat row doesn't exist...
    if (chat[curLine].isNull())
        return false;
    QHash<QString, QVariant> chatRow = chat[curLine].toHash();
    // Reinsert
    chatRow["nice"] = nice;
    chat[curLine] = chatRow;
    entry["chat"] = chat;
    // Reinsert
    data[curEntry] = entry;
    final["data"] = data;

    return true;
}

/**
 * Getter: Get Protocol
 * @returns QString The protocol name
 */
QString StdFormat::getProtocol()
{
    QString curEntry;
    curEntry.setNum(dexEntry);
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    // If selected entry doesn't exist...
    if (data[curEntry].isNull())
        return false;
    QHash<QString, QVariant> entry = data[curEntry].toHash();
    return entry["protocol"].toString();
}

/**
 * Getter: Get Self
 * @returns QString The account name of the primary person
 */
QString StdFormat::getSelf()
{
    QString curEntry;
    curEntry.setNum(dexEntry);
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    // If selected entry doesn't exist...
    if (data[curEntry].isNull())
        return false;
    QHash<QString, QVariant> entry = data[curEntry].toHash();
    return entry["self"].toString();
}

/**
 * Getter: Get Self Alias
 * @returns QString The self account display name to get
 */
QString StdFormat::getSelfAlias()
{
    QString curEntry;
    curEntry.setNum(dexEntry);
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    // If selected entry doesn't exist...
    if (data[curEntry].isNull())
        return false;
    QHash<QString, QVariant> entry = data[curEntry].toHash();
    return entry["self_alias"].toString();
}

/**
 * Getter: Get With
 * @returns QString The primary buddy account username to get
 */
QString StdFormat::getWith()
{
    QString curEntry;
    curEntry.setNum(dexEntry);
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    // If selected entry doesn't exist...
    if (data[curEntry].isNull())
        return false;
    QHash<QString, QVariant> entry = data[curEntry].toHash();
    return entry["with"].toString();
}

/**
 * Getter: Get With Alias
 * @returns QString The primary buddy account display name to get
 */
QString StdFormat::getWithAlias()
{
    QString curEntry;
    curEntry.setNum(dexEntry);
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    // If selected entry doesn't exist...
    if (data[curEntry].isNull())
        return false;
    QHash<QString, QVariant> entry = data[curEntry].toHash();
    return entry["with_alias"].toString();
}

/**
 * Getter: Get Time
 * @returns int UNIX epoch of the time to get
 */
qint64 StdFormat::getTime()
{
    QString curEntry;
    QString curLine;
    curEntry.setNum(dexEntry);
    curLine.setNum(dexLine);
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    // If selected entry doesn't exist...
    if (data[curEntry].isNull())
        return false;
    QHash<QString, QVariant> entry = data[curEntry].toHash();
    // Default (entry) timezone or chat row timezone? inLine decides.
    if (!inLine)
    {
        return entry["time"].toInt();
    }
    // If the entry hasn't initialized any chat rows...
    if (entry["chat"].isNull())
        return false;
    QMap<QString, QVariant> chat = entry["chat"].toMap();
    // If the set chat row doesn't exist...
    if (chat[curLine].isNull())
        return false;
    QHash<QString, QVariant> chatRow = chat[curLine].toHash();
    // Get time
    return chatRow["time"].toLongLong();
}

/**
 * Getter: Get Timezone
 * @returns QString Timezone identifier to accompany the UNIX time
 */
QString StdFormat::getTimezone()
{
    QString curEntry;
    QString curLine;
    curEntry.setNum(dexEntry);
    curLine.setNum(dexLine);
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    // If selected entry doesn't exist...
    if (data[curEntry].isNull())
        return false;
    QHash<QString, QVariant> entry = data[curEntry].toHash();
    // Default (entry) timezone or chat row timezone? inLine decides.
    if (!inLine)
    {
        return entry["timezone"].toString();
    }
    // If the entry hasn't initialized any chat rows...
    if (entry["chat"].isNull())
        return false;
    QMap<QString, QVariant> chat = entry["chat"].toMap();
    // If the set chat row doesn't exist...
    if (chat[curLine].isNull())
        return false;
    QHash<QString, QVariant> chatRow = chat[curLine].toHash();
    // Get timezone
    return chatRow["timezone"].toString();
}

/**
 * Getter: Get Sender
 * @returns QString Username of the sender
 */
QString StdFormat::getSender()
{
    QString curEntry;
    QString curLine;
    curEntry.setNum(dexEntry);
    curLine.setNum(dexLine);
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    // If selected entry doesn't exist...
    if (data[curEntry].isNull())
        return false;
    QHash<QString, QVariant> entry = data[curEntry].toHash();
    // If the entry hasn't initialized any chat rows...
    if (entry["chat"].isNull())
        return false;
    QMap<QString, QVariant> chat = entry["chat"].toMap();
    // If the set chat row doesn't exist...
    if (chat[curLine].isNull())
        return false;
    QHash<QString, QVariant> chatRow = chat[curLine].toHash();
    // Get sender
    return chatRow["sender"].toString();
}

/**
 * Getter: Get Sender Alias
 * @returns QString Alias of the sender
 */
QString StdFormat::getAlias()
{
    QString curEntry;
    QString curLine;
    curEntry.setNum(dexEntry);
    curLine.setNum(dexLine);
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    // If selected entry doesn't exist...
    if (data[curEntry].isNull())
        return false;
    QHash<QString, QVariant> entry = data[curEntry].toHash();
    // If the entry hasn't initialized any chat rows...
    if (entry["chat"].isNull())
        return false;
    QMap<QString, QVariant> chat = entry["chat"].toMap();
    // If the set chat row doesn't exist...
    if (chat[curLine].isNull())
        return false;
    QHash<QString, QVariant> chatRow = chat[curLine].toHash();
    // Get alias
    return chatRow["alias"].toString();
}

/**
 * Getter: Get Message Content
 * @returns QString Content of the message
 */
QString StdFormat::getContent()
{
    QString curEntry;
    QString curLine;
    curEntry.setNum(dexEntry);
    curLine.setNum(dexLine);
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    // If selected entry doesn't exist...
    if (data[curEntry].isNull())
        return false;
    QHash<QString, QVariant> entry = data[curEntry].toHash();
    // If the entry hasn't initialized any chat rows...
    if (entry["chat"].isNull())
        return false;
    QMap<QString, QVariant> chat = entry["chat"].toMap();
    // If the set chat row doesn't exist...
    if (chat[curLine].isNull())
        return false;
    QHash<QString, QVariant> chatRow = chat[curLine].toHash();
    // Get content
    return chatRow["content"].toString();
}

/**
 * Getter: Get Log2Log Message Code
 * @returns int Code ID
 */
int StdFormat::getCode()
{
    QString curEntry;
    QString curLine;
    curEntry.setNum(dexEntry);
    curLine.setNum(dexLine);
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    // If selected entry doesn't exist...
    if (data[curEntry].isNull())
        return false;
    QHash<QString, QVariant> entry = data[curEntry].toHash();
    // If the entry hasn't initialized any chat rows...
    if (entry["chat"].isNull())
        return false;
    QMap<QString, QVariant> chat = entry["chat"].toMap();
    // If the set chat row doesn't exist...
    if (chat[curLine].isNull())
        return false;
    QHash<QString, QVariant> chatRow = chat[curLine].toHash();
    // Get code
    return chatRow["code"].toInt();
}

/**
 * Getter: Get Log2Log Timestamp Specificity Index
 * @returns int Log2Log Timestamp Specificity Index
 */
int StdFormat::getSpecificity()
{
    QString curEntry;
    QString curLine;
    curEntry.setNum(dexEntry);
    curLine.setNum(dexLine);
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    // If selected entry doesn't exist...
    if (data[curEntry].isNull())
        return false;
    QHash<QString, QVariant> entry = data[curEntry].toHash();
    // If the entry hasn't initialized any chat rows...
    if (entry["chat"].isNull())
        return false;
    QMap<QString, QVariant> chat = entry["chat"].toMap();
    // If the set chat row doesn't exist...
    if (chat[curLine].isNull())
        return false;
    QHash<QString, QVariant> chatRow = chat[curLine].toHash();
    // Get specificity
    return chatRow["specificity"].toInt();
}

/**
 * Getter: Set Log2Log Message Content Accuracy Index
 * @returns int Log2Log Message Content Accuracy Index
 */
int StdFormat::getAccuracy()
{
    QString curEntry;
    QString curLine;
    curEntry.setNum(dexEntry);
    curLine.setNum(dexLine);
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    // If selected entry doesn't exist...
    if (data[curEntry].isNull())
        return false;
    QHash<QString, QVariant> entry = data[curEntry].toHash();
    // If the entry hasn't initialized any chat rows...
    if (entry["chat"].isNull())
        return false;
    QMap<QString, QVariant> chat = entry["chat"].toMap();
    // If the set chat row doesn't exist...
    if (chat[curLine].isNull())
        return false;
    QHash<QString, QVariant> chatRow = chat[curLine].toHash();
    // Get accuracy
    return chatRow["accuracy"].toInt();
}

/**
 * Getter: Get Log2Log Message Niceness
 * @returns int Log2Log message nice value
 */
int StdFormat::getNice()
{
    QString curEntry;
    QString curLine;
    curEntry.setNum(dexEntry);
    curLine.setNum(dexLine);
    // Extract
    QMap<QString, QVariant> data = final["data"].toMap();
    // If selected entry doesn't exist...
    if (data[curEntry].isNull())
        return false;
    QHash<QString, QVariant> entry = data[curEntry].toHash();
    // If the entry hasn't initialized any chat rows...
    if (entry["chat"].isNull())
        return false;
    QMap<QString, QVariant> chat = entry["chat"].toMap();
    // If the set chat row doesn't exist...
    if (chat[curLine].isNull())
        return false;
    QHash<QString, QVariant> chatRow = chat[curLine].toHash();
    // Get nice
    return chatRow["nice"].toInt();
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
            (int)"specificity" -> an index of how accurate the time is; ex. 2
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

Log2Log Timestamp Specificity Index
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
