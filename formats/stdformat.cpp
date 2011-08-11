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
 *          final.value("data").setValue(extractedData);
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
 * Set Client
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
        "timezone" -> the start time's timeznoe; ex. America/Chicago
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

*/
