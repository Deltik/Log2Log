/**
 * Log2Log
 *  Formats
 *   Meebo (download)
 *
 * @author Deltik
 * @author igordcard
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

/**
 * #### CONTENTS ####
 * # Class Variables   - "Meebo Connect Variable Declarations"
 * # Class Constructor - "Meebo Connect Class Constructor"
 * # Low-Level Access  - "Meebo API Raw Access Functions"
 * # Standard Access   - "Meebo API Normal Access Functions"
 * # High-Level Access - "Meebo API Sophisticated Access Functions"
 * # Advanced Features - "Log2Log Functions"
 * ##################
 */

#include "meeboconnect.h"
#include "helper.h"
#include <QtCore>
#include <QtXml>
#include <QDateTime>

/**
 * Constructor
 */

MeeboConnect::MeeboConnect()
{
    final = new StdFormat();
    final->setClient("Meebo");
    api = new Api();
}


/**
 * ##################################
 * # Meebo API Raw Access Functions #
 * ##################################
 */

/**
 * Top-level API accessor commands
 * @param QString func API's function
 * @param QHash<QString, QString> params Parameters to set in the API function
 * @param QNetworkAccessManager::Operation Operation of QNetworkAccessManager
 * @param bool https Set to TRUE for HTTPS access mode
 * @param bool mcmd Whether to use Meebo's MCMD or CMD API
 * @param Api apporter The API handler to use
 */
void MeeboConnect::accessCMD(QString func, QHash<QString, QString> params, QNetworkAccessManager::Operation op, bool https, bool mcmd, Api *apporter)
{
    if (mcmd)
        func = "mcmd/" + func;
    else
        func = "cmd/" + func;

    return this->accessMeebo(func, params, op, https, apporter);
}

/**
 * Accesses an JSON-API functions through this,
 * processes data for accessing the API,
 * accesses the API,
 * and returns the output.
 */
void MeeboConnect::accessMeebo(QString func, QHash<QString, QString> params, QNetworkAccessManager::Operation op, bool https, Api *apporter)
{
    if (op == QNetworkAccessManager::PostOperation)
    {
        QHashIterator<QString, QString> i(params);
        i.toBack();
        while (i.hasPrevious())
        {
            i.previous();

            api->addPost(i.key(), i.value());
        }
    }
    else
    {
        QUrl get;
        QHashIterator<QString, QString> i(params);
        i.toBack();
        while (i.hasPrevious())
        {
            i.previous();
            get.addQueryItem(i.key(), i.value());
        }
        func += "?" + get.encodedQuery();
    }

    this->accessAPI(func, https, apporter);
}

/**
 * Accesses the Meebo API directly.
 */
void MeeboConnect::accessAPI(QString command, bool https, Api *apporter)
{
    // Set default API handler
    if (apporter == NULL)
        apporter = api;

    QString url;
    QString s = "";
    if (https == true)
        s = "s";
    url = "http"+s+"://www.meebo.com/" + command;
    apporter->setURL(url);

    // Fake Web browser settings

    //connect(api, SIGNAL(requestComplete(QString)), this, SLOT(interpretReply(QString)));

    apporter->start();
    apporter->wait();
    interpretReply(apporter->str);
}

/**
 * Interpret API reply
 */
void MeeboConnect::interpretReply(QString reply)
{
    response = reply;
    emit apiReply(reply);
}


/**
 * #####################################
 * # Meebo API Normal Access Functions #
 * #####################################
 */

/**
 * Start a Meebo session
 * @param QString bcookie (optional) Random beginning cookie identifier
 * @returns array
 */
QMap<QString, QVariant> MeeboConnect::startAPI(QString bcookie)
{
    // Set unique Log2Log/Meebo ID, if not already given
    if (bcookie.isEmpty())
        bcookie = "log2log-" +
                  QString(QCryptographicHash::hash(QVariant(QDateTime::currentMSecsSinceEpoch()).toByteArray(), QCryptographicHash::Md5).toHex().constData());

    // Set up parameters to send with the function
    QHash<QString, QString> params;
    params.insert("type", "core");
    params.insert("bcookie", bcookie);
    params.insert("ts", QVariant(QDateTime::currentMSecsSinceEpoch()).toString());

    //connect(this, SIGNAL(apiReply(QString)), SLOT(startAPIReply(QString)));

    // Access API!
    this->accessCMD("start", params, QNetworkAccessManager::GetOperation, true, true);

    // Get reply
    QMap<QString, QVariant> data = Json::parse(response).toMap();
    // Set session variables
    this->sessionKey = data["sessionKey"].toString();
    this->sessionId  = data["sessionId"] .toLongLong();
    this->clientId   = data["clientId"]  .toLongLong();
    this->revision   = 0;

    // Return the response, just to be nice. ;)
    return data;
}

/**
 * Get updates from Meebo
 * @param qint32 rev (optional) Update revision request number
 * @param QString sessionKey (optional) Meebo's generated unique session ID
 * @param qint64 clientId (optional) Meebo's generated unique client ID
 * @param qint32 focustime (optional) [WHAT IN THE WORLD DOES THIS DO?!]
 */
QMap<QString, QVariant> MeeboConnect::updateAPI(qint32 rev, QString sessionKey, qint64 clientId, qint32 focustime)
{
    // Parameter Defaults
    if (!rev) rev = this->revision;
    if (sessionKey.isEmpty()) sessionKey = this->sessionKey;
    if (!clientId) clientId = this->clientId;
    if (!focustime) focustime = rev;

    // Set up parameters to send with the function
    QHash<QString, QString> params;
    params.insert("sessionKey", sessionKey);
    params.insert("rev", QVariant(rev).toString());
    params.insert("clientId", QVariant(clientId).toString());
    params.insert("focustime", QVariant(focustime).toString());

    //connect(this, SIGNAL(apiReply(QString)), SLOT(updateAPIReply(QString)));

    // Access API! (Important: Use the updateApi API handler for asynchronism.)
    updateApi = new Api();
    this->accessCMD("events", params, QNetworkAccessManager::GetOperation, true, true, updateApi);

    // Get reply
    QMap<QString, QVariant> data = Json::parse(response).toMap();
    // Set the session variables
    this->revision = data["rev"].toInt();

    // Return the response
    return data;
}
/// Looping Update
void MeeboConnect::updateCycle()
{
    qint64 temp = QDateTime::currentMSecsSinceEpoch();
    qDebug() << "Update Cycle!";
    this->updateAPI();
    qint64 temp2 = QDateTime::currentMSecsSinceEpoch();
    qint64 timetook = temp2 - temp;
    qDebug() << "Update Cycle Complete! Took: " + QVariant((double)timetook/1000).toString() + "s";
}

/**
 * Log in to Meebo account
 * @param QString username Meebo ID
 * @param QString password The password required to connect to a Meebo ID
 * @param QString sessionKey (optional) Meebo's generated unique session ID
 * @param qint64 clientId (optional) Meebo's generated unique client ID
 */
void MeeboConnect::loginAPI(QString username, QString password, QString sessionKey, qint64 clientId)
{
    // Parameter Defaults
    if (username.isEmpty()) username = this->username;
    if (password.isEmpty()) password = this->password;
    if (sessionKey.isEmpty()) sessionKey = this->sessionKey;
    if (!clientId) clientId = this->clientId;

    // Set up parameters to send with the function
    QHash<QString, QString> params;
    params.insert("sessionKey", sessionKey);
    params.insert("requestId", "F0");
    params.insert("clientId", QVariant(clientId).toString());
    params.insert("state", "invisible");
    params.insert("num", "1");
    params.insert("username0", username);
    params.insert("protocol0", "meebo");
    params.insert("password0", password);

    // Access API!
    this->accessCMD("joinexisting", params, QNetworkAccessManager::PostOperation, true, true);
}

/**
 * Disconnect from Meebo
 * @param QString sessionKey (optional) Meebo's generated unique session ID
 * @param qint64 clientId (optional) Meebo's generated unique client ID
 */
void MeeboConnect::quitAPI(QString sessionKey, qint64 clientId)
{
    // Parameter Defaults
    if (sessionKey.isEmpty()) sessionKey = this->sessionKey;
    if (!clientId) clientId = this->clientId;

    // Set up parameters to send with the function
    QHash<QString, QString> params;
    params.insert("sessionKey", sessionKey);
    params.insert("clientId", QVariant(clientId).toString());

    // Access API!
    this->accessCMD("quit", params, QNetworkAccessManager::GetOperation, true, true);
}

/**
 * Sign Off of Meebo
 * @param QString sessionKey (optional) Meebo's generated unique session ID
 * @param qint64 locationId (optional) Meebo's generated unique client ID
 * @param qint64 clientId (optional) Meebo's generated unique client ID
 */
void MeeboConnect::signOffAPI(QString sessionKey, qint64 locationId, qint64 clientId, bool clearSLT)
{
    // Parameter Defaults
    if (sessionKey.isEmpty()) sessionKey = this->sessionKey;
    if (!locationId) clientId = this->clientId;
    QString mode; if (clearSLT) mode = "true"; else mode = "false";
    if (!clientId) clientId = this->clientId;

    // Set up parameters to send with the function
    QHash<QString, QString> params;
    params.insert("sessionKey", sessionKey);
    params.insert("locationId", QVariant(locationId).toString());
    params.insert("clearSLT", mode);
    params.insert("clientId", QVariant(clientId).toString());

    // Access API!
    this->accessCMD("detach", params, QNetworkAccessManager::GetOperation, true, true);
}

/**
 * CMD: "mauserlist" (I have no idea what this does.)
 * @param QString sessionKey (optional) Meebo's generated unique session ID
 * @param qint64 clientId (optional) Meebo's generated unique client ID
 * @param QString username_meebo Meebo username associated with sessionKey
 */
void MeeboConnect::mauserlistAPI(QString sessionKey, qint64 clientId, QString username_meebo)
{
    // Parameter Defaults
    if (username_meebo.isEmpty()) username = this->username;
    if (sessionKey.isEmpty()) sessionKey = this->sessionKey;
    if (!clientId) clientId = this->clientId;

    // Set up parameters to send with the function
    QHash<QString, QString> params;
    params.insert("sessionKey", sessionKey);
    params.insert("muser", QVariant(username_meebo).toString());
    params.insert("clientId", QVariant(clientId).toString());

    // Access API!
    this->accessCMD("mauserlist", params, QNetworkAccessManager::PostOperation, true, false);
}

/**
 * CMD: "gwid" (I have no idea what this does.)
 * @param QString sessionKey (optional) Meebo's generated unique session ID
 * @param qint64 clientId (optional) Meebo's generated unique client ID
 * @param QString username_meebo Meebo username associated with sessionKey
 */
void MeeboConnect::gwidAPI(QString sessionKey, qint64 clientId, QString username_meebo)
{
    // Parameter Defaults
    if (username_meebo.isEmpty()) username = this->username;
    if (sessionKey.isEmpty()) sessionKey = this->sessionKey;
    if (!clientId) clientId = this->clientId;

    // Set up parameters to send with the function
    QHash<QString, QString> params;
    params.insert("sessionKey", sessionKey);
    params.insert("muser", QVariant(username_meebo).toString());
    params.insert("clientId", QVariant(clientId).toString());

    // Access API!
    this->accessCMD("gwid", params, QNetworkAccessManager::PostOperation, true, false);
}

/**
 * MCMD: "dbg" (I have no idea what this is.)
 * @param QString data Requested information from 'dbg'
 * @param QString category (optional) Category of data received
 * @param QString sessionKey (optional) Meebo's generated unique session ID
 * @param qint64 clientId (optional) Meebo's generated unique client ID
 */
void MeeboConnect::dbgAPI(QString data, QString category, QString sessionKey, qint64 clientId)
{
    // Parameter Defaults
    if (sessionKey.isEmpty()) sessionKey = this->sessionKey;
    if (!clientId) clientId = this->clientId;

    // Set up parameters to send with the function
    QHash<QString, QString> params;
    params.insert("sessionKey", sessionKey);
    params.insert("cat", QVariant(category).toString());
    params.insert("m", data);
    params.insert("clientId", QVariant(clientId).toString());

    // Access API!
    this->accessCMD("dbg", params, QNetworkAccessManager::GetOperation, true, true);
}

/**
 * Make a Request for Meebo Preferences
 */
void MeeboConnect::prefgetAPI(QList<QVariant> items, QString sessionKey, qint64 clientId)
{
    // Parameter Defaults
    if (sessionKey.isEmpty()) sessionKey = this->sessionKey;
    if (!clientId) clientId = this->clientId;
    if (items.isEmpty()) { for (int i = 0; i <= 4; i ++) items << "wallpaper" + QVariant(i).toString(); }

    // Set up parameters to send with the function
    QHash<QString, QString> params;
    params.insert("clientId", QVariant(clientId).toString());
    for (int i = 0; i < items.size(); i ++)
    {
        params.insert("name" + QVariant(i).toString(), items[i].toString());
    }
    params.insert("num", QVariant(items.size()).toString());
    params.insert("sessionKey", sessionKey);

    // Access API!
    this->accessCMD("prefget", params, QNetworkAccessManager::PostOperation, true, true);
}

/**
 * Load into the Server some Buddy Information
 * @param QString username_with The other chatter's username
 * @param QString username_self Account to which the contact is assigned
 * @param QString protocol Protocol to which the contact is on
 * @param QString sessionKey (optional) Meebo's generated unique session ID
 * @param qint64 clientId (optional) Meebo's generated unique client ID
 */
void MeeboConnect::infoAPI(QString username_with, QString username_self, QString protocol, QString sessionKey, qint64 clientId)
{
    // Parameter Defaults
    if (sessionKey.isEmpty()) sessionKey = this->sessionKey;
    if (!clientId) clientId = this->clientId;

    // Set up parameters to send with the function
    QHash<QString, QString> params;
    params.insert("sessionKey", sessionKey);
    params.insert("u", username_self);
    params.insert("p", protocol);
    params.insert("b", username_with);
    params.insert("clientId", QVariant(clientId).toString());

    // Access API!
    this->accessCMD("info", params, QNetworkAccessManager::PostOperation, true, true);

}

/**
 * Get Chat Log
 * @param QString username_with The other chatter's username
 * @param QString username_self Account to which the log is assigned
 * @param QString protocol Protocol to which the log is assigned
 * @param QString username_meebo Meebo username associated with sessionKey
 * @param QString sessionKey (optional) Meebo's generated unique session ID
 * @param QString clientId (optional) Meebo's generated unique client ID
 * @returns QString The messed up chat log that needs help from Log2Log
 */
QString MeeboConnect::getChatLogAPI(QString username_with, QString username_self, QString protocol, QString username_meebo, QString sessionKey, qint64 clientId)
{
    // Parameter Defaults
    if (username_meebo.isEmpty()) username_meebo = this->username;
    if (sessionKey.isEmpty()) sessionKey = this->sessionKey;
    if (!clientId) clientId = this->clientId;

    // Set up parameters to send with the function
    QHash<QString, QString> params;
    params.insert("bu", username_with);
    params.insert("clientId", QVariant(clientId).toString());
    params.insert("muser", username_meebo);
    params.insert("p", protocol);
    params.insert("sessionKey", sessionKey);
    params.insert("u", username_self);

    // Access API!
    this->accessCMD("cl_proxy", params, QNetworkAccessManager::PostOperation, true, false);qDebug()<<"LOGDATASTARTSWITH: "+response.left(128);

    // Return the response
    return response;
}

/**
 * Alternate Get Chat Log (for Meebo Mobile Phone App)
 * @param QString username_with The other chatter's username
 * @param QString username_self Account to which the log is assigned
 * @param QString protocol Protocol to which the log is assigned
 * @param QString username_meebo Meebo username associated with sessionKey
 * @param QString timePeriod Conversation entry ID, defaults to "latest"
 * @param QString sessionKey (optional) Meebo's generated unique session ID
 * @returns QString An HTML representation of a chat log entry for the Meebo mobile phone app
 */
QString MeeboConnect::getChatLogAlt(QString username_with, QString username_self, QString protocol, QString username_meebo, QString timePeriod, QString sessionKey)
{
    // Parameter Defaults
    if (username_meebo.isEmpty()) username_meebo = this->username;
    if (sessionKey.isEmpty()) sessionKey = this->sessionKey;
    if (timePeriod.isEmpty()) timePeriod = "latest";

    // Access Logs Stash!
    api->setURL("https://logs.meebo.com/chatlogs/" +
                QUrl::toPercentEncoding(username_meebo) +
                "/" +
                QUrl::toPercentEncoding(protocol) +
                "/" +
                QUrl::toPercentEncoding(username_self) +
                "/" +
                QUrl::toPercentEncoding(username_with) +
                "/" +
                QUrl::toPercentEncoding(timePeriod) +
                "/?k=" +
                sessionKey);
    api->start();
    api->wait();
    interpretReply(api->str);qDebug()<<"LOGDATASTARTSWITH: "+response;

    // Return the response
    return response;
}


/**
 * ############################################
 * # Meebo API Sophisticated Access Functions #
 * ############################################
 */

/**
 * Collect Initial Meebo Account Data
 * @param QString username Meebo ID
 * @param QString password The password required to connect to a Meebo ID
 * @returns array Collected data
 */
void MeeboConnect::initialize(QString username, QString password)
{
    // Parameter Defaults
    if (username.isEmpty()) username = this->username;
    if (password.isEmpty()) password = this->password;

    // Get a session key from Meebo.
    this->updateProgress(0, "Asking Meebo for a session key...");
    this->startAPI();

    // Log in.
    this->updateProgress(0, "Logging in...");
    this->loginAPI(username, password);

    // Collect from info::attached
    this->updateProgress(0, "Getting a list of your accounts...");
    QMap<QString, QVariant> temp = this->updateAPI();qDebug()<<response;
    this->parseContacts(temp);

    // cmd/gwid and mcmd/dbg (I don't know what those do.)
    this->gwidAPI();
    this->dbgAPI();

    // Here's where it gets difficult.
    // Capture data.
    while (revision < 5)
    {
        // Trickery and deception
        if (revision == 3)
        {
            this->gwidAPI();
            this->dbgAPI();
            this->prefgetAPI();
        }
        // Get next update.
        QMap<QString, QVariant> temp = this->updateAPI();qDebug()<<response;

        // Detect incorrect authentication.
        if (response.contains("\"protocol\":\"meebo\",\"data\":{\"type\":1,\"description\":\"Username does not exist\"}}}"))
        {
            errorText = "Incorrect username"; return;
        }
        else if (response.contains("\"protocol\":\"meebo\",\"data\":{\"type\":2,\"description\":\"Incorrect password\"}}}"))
        {
            errorText = "Incorrect password"; return;
        }
        else if (response.contains("\"protocol\":\"meebo\",\"data\":{\"type\":2,\"description\":\"Incorrect username or password.\"}}}"))
        {
            errorText = "Incorrect username or password"; return;
        }
        else if (response.contains("\"data\":{\"stat\":\"fail\",\"msg\":\"Invalid request\",\"errorcode\":3}"))
        {
            errorText = "Bad login information"; return;
        }

        // Parse contacts from a Meebo "events".
        this->parseContacts(temp);
        updateProgress(NULL, "Cycle #" + QVariant(revision).toString() + ", counted " + QVariant(contacts.size()).toString() + " buddies");
    }

    //  Launch event cycler
    updateCycler = new QTimer();
    updateCycler->setInterval(0);
    connect(updateCycler, SIGNAL(timeout()), this, SLOT(updateCycle()));
    updateCycler->start();
}


/**
 * #####################
 * # Log2Log Functions #
 * #####################
 */

/**
 * Filter to Buddy List
 * @param QVariantMap data Data collected by this->initialize()
 */
void MeeboConnect::parseContacts(QMap<QString, QVariant> data)
{
    QList<QVariant> updates = data["events"].toList();

    // For each update event...
    for (int i = 0; i < updates.size(); i ++)
    {
        // Extract update
        QMap<QString, QVariant> update = updates.at(i).toMap();
        QMap<QString, QVariant> data   = update["data"].toMap();

        // Process the type of event
        QStringList type_split = update["type"].toString().split("::");
        QString event_category, event_type = "";
        if (!type_split.isEmpty())
            event_category = type_split.takeFirst();
        if (!type_split.isEmpty())
            event_type     = type_split.takeLast();

        // # Extract External Session? #
        if (event_category == "info" &&
            event_type     == "external_session")
        {
            bool thereIsAnExternalSession = false;
            QMap<QString, QVariant> data_ext = this->pullExternalSessionEvents(data, thereIsAnExternalSession);
            if (thereIsAnExternalSession)
            {
                this->parseContacts(data_ext);
                break;
            }
        }

        // # Extract Account Information #
        if (event_category == "account")
        {
            if (event_type == "info" ||
                event_type == "connecting" ||
                event_type == "alias_changed" ||
                event_type == "online")
            {
                QString $_account_username = data["user"].toString();
                QString $_account_protocol = data["protocol"].toString();
                QString $_account_alias    = "";
                if (event_type == "info" || event_type == "connecting")
                    $_account_alias        = data["alias"].toString();
                else // TODO: Confirm that the next line is correct.
                    $_account_alias        = data["data"].toString();
                QMap<QString, QVariant> account = accounts[$_account_username].toMap();
                account["protocol"] = $_account_protocol;
                account["alias"]    = $_account_alias;
                accounts[$_account_username] = account;
            }
        }

        // # Extract Buddy Information
        if (event_category == "buddy")
        {
            QString $_buddy_account_assoc = data["user"].toString();
            QString $_buddy_username      = data["buddy"].toString();
            QString $_buddy_alias         = data["buddyalias"].toString();
            QString $_buddy_protocol      = data["protocol"].toString();
            // Verify that the buddy is not being repeated before adding it.
            bool add = true;
            for (int i = 0; i < contacts.size(); i ++)
            {
                if (contacts[i].value("account_assoc") == $_buddy_account_assoc &&
                    contacts[i].value("username") == $_buddy_username &&
                    contacts[i].value("protocol") == $_buddy_protocol)
                {
                    add = false;
                    break;
                }
            }
            if (add)
            {
                QMap<QString, QVariant> buddy;
                buddy["account_assoc"] = $_buddy_account_assoc;
                buddy["username"]      = $_buddy_username;
                buddy["alias"]         = $_buddy_alias;
                buddy["protocol"]      = $_buddy_protocol;
                contacts << buddy;
            }
        }
    }
}

/**
 * Collect All Chat Logs based on "contacts" list
 */
void MeeboConnect::getAllChatLogs()
{
    for (int i = 0; i < contacts.size(); i ++)
    {
        QMap<QString, QVariant> contact = contacts[i];
        QString alias = contact["alias"].toString();
        if (alias.trimmed().isEmpty())
            alias = contact["username"].toString();
        updateProgress((25 * i / contacts.size()),
                       "Downloading chat log " +
                       QVariant(i+1).toString() +
                       "/" + QVariant(contacts.size()).toString() +
                       "... (" +
                       alias +
                       " on " +
                       contact["protocol"].toString() +
                       ")");
        contact["rawlog"] = this->getChatLogAPI(contact["username"].toString(), contact["account_assoc"].toString(), contact["protocol"].toString());
        contacts[i] = contact;
    }
}

/**
 * Pull Data from External Session If Necessary
 *  AKA Log2Log Compatibility When Logged In Elsewhere
 *  AKA Meebo Events Loaded from another Session
 * @param QVariantMap data Data collected by this->initialize()
 * @param bool &success The Boolean to indicate if there are external events
 * @returns QVariantMap The pulled data from the external session
 */
QMap<QString, QVariant> MeeboConnect::pullExternalSessionEvents(QMap<QString, QVariant> data, bool &success)
{
    if (!data.value("data").isNull())
    {
        success = true;
        return data["data"].toMap();
    }
    return QVariantMap();
}

/********/
// All the chat logs have been fetched, stop the timer and finish the work
void MeeboConnect::gotAllChatLogs()
{
    updateCycler->stop();

    //  Bail out of Meebo
    updateProgress(25, "Signing off...");
    this->signOffAPI();
    //updateCycler->stop();

    // Step 2/3: Process the data through the format converter "Meebo".
    //           (This class extends the "Meebo" class.)
    for (int i = 0; i < contacts.size(); i ++)
    {
        QMap<QString, QVariant> contact = contacts[i];

        this->setAccount(contact.value("account_assoc").toString());
        this->setProtocol(contact.value("protocol").toString());
        this->setWith(contact.value("username").toString());

        this->load(contact.value("rawlog"));
        updateProgress((25 * (i+1) / contacts.size()) + 25, "Interpreted " + QVariant(i+1).toString() + "/" + QVariant(contacts.size()).toString() + " logs...");
    }

    // Step 3/3: Submit the Log2Log-standardized chat log array.
    emit finished();
}
/********/

/**
 * Process "From" Request
 */
StdFormat* MeeboConnect::from(QHash<QString, QVariant> data)
{
    // Step 1/3: Fetch the data.
    username = data["username"].toString();
    password = data["password"].toString();
    //  Authentication
    this->initialize(username, password);
    //  Catch error during authentication
    if (!errorText.isNull())
    {
        emit error(errorText);
        return new StdFormat();
    }

    QFuture<void> *future = new QFuture<void>();
    QFutureWatcher<void> *watcher = new QFutureWatcher<void>();

    // Create a new thread dedicated to get all the chat logs
    // While that happens, the timer will keep running, ensuring connectivity with Meebo
    *future = QtConcurrent::run(this, &MeeboConnect::getAllChatLogs);
    watcher->setFuture(*future);
    // When all the chat logs are fetched, send a signal advising so
    connect(watcher, SIGNAL(finished()), this, SLOT(gotAllChatLogs()));

    // TODO: this architecture needs to be adapted to the new getAllChatLogs() thread and the gotAllChatLogs() slot
    return this->final; //
}
