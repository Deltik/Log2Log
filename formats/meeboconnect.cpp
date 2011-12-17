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
    chatLogsAreDownloadingAlready = false;
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
QString MeeboConnect::accessCMD(QString func, QHash<QString, QString> params, QNetworkAccessManager::Operation op, bool https, bool mcmd, Api *apporter)
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
QString MeeboConnect::accessMeebo(QString func, QHash<QString, QString> params, QNetworkAccessManager::Operation op, bool https, Api *apporter)
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

    return this->accessAPI(func, https, apporter);
}

/**
 * Accesses the Meebo API directly.
 * @return The interpreted API result
 */
QString MeeboConnect::accessAPI(QString command, bool https, Api *apporter)
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

    apporter->start();
    apporter->wait();
    return interpretReply(apporter->str);
}

/**
 * Interpret API reply
 * @returns The downloaded API content
 */
QString MeeboConnect::interpretReply(QString reply)
{
    response = reply;
    emit apiReply(reply);
    return reply;
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

    // Access API!
    QString result = this->accessCMD("start", params, QNetworkAccessManager::GetOperation, true, true);

    // Get reply
    QMap<QString, QVariant> data = Json::parse(result).toMap();
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
 * @param Api updateApi Asynchronized API handler to use
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

    // Access API! (Important: Use the updateApi API handler for asynchronism.)
    updateApi = new Api();
    updateApi->hed = api->hed;
    QString result = this->accessCMD("events", params, QNetworkAccessManager::GetOperation, true, true, updateApi);

    // Get reply
    QMap<QString, QVariant> data = Json::parse(result).toMap();
    // Set the session variables
    this->revision = data["rev"].toInt();
    // Include update data in the QMap
    data["raw"] = result;

    // Return the response
    return data;
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
    QString result = this->accessCMD("cl_proxy", params, QNetworkAccessManager::PostOperation, true, false);
    /* DEBUG */qDebug() << "LOGDATASTARTSWITH: " + result.left(512);

    // Return the raw chat log
    return result;
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
    QString result = interpretReply(api->str);

    // Return the response
    return result;
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

    //  Launch event cycler
    this->updateProgress(0, "Waiting for data...");
    updater = new MeeboConnectKeepAlive(this);
    connect(updater, SIGNAL(updateAPIReply(QMap<QString,QVariant>)), SLOT(updateAPIHandler(QMap<QString,QVariant>)));
    connect(updater, SIGNAL(updateAPIError(QString)), SLOT(abort(QString)));
    updater->start();
}


/**
 * #####################
 * # Log2Log Functions #
 * #####################
 */

/**
 * Handle Meebo "events"
 * @param QVariantMap data "events" from Meebo, JSON-parsed
 */
void MeeboConnect::updateAPIHandler(QMap<QString, QVariant> data)
{
    // Always try to parse contacts, even if there are none in this update.
    this->parseContacts(data);
}

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

        // # Successfully Logged In #
        if (event_category == "info" &&
            event_type     == "attached")
        {
            // Broadcast: "Logged In"
            emit updateAPIStatusConnected();
        }

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

                // Broadcast: "Account Found"
                emit updateAPIStatusAccounts();
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

                // Broadcast: "Contact Found"
                emit updateAPIStatusBuddies();
            }
        }
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
    success = false;
    return QVariantMap();
}

/**
 * Start Downloading Chat Logs
 */
void MeeboConnect::startDownloadingChatLogs()
{
    // If this function has been called once before, refuse to run again.
    if (chatLogsAreDownloadingAlready)
        return;

    // Lock this function so that it can only execute once.
    chatLogsAreDownloadingAlready = true;

    // Announce to the client that the download is starting.
    updateProgress(0, "If this message persists, the download failed to start.");

    /// Get all chat logs! :D

    // Create the downloader instance.
    downloader = new MeeboConnectDownloader(this);

    // Link signals from the downloader.
    connect(downloader, SIGNAL(updateProgress(int, QString)), this, SLOT(passProgressProto(int, QString)));
    connect(downloader, SIGNAL(chatLogsDownloaded()), waiter, SLOT(quit()));

    // Start downloading all chat logs
    downloader->start();
}

/**
 * Abort Log2Log Meebo Downloader!!!
 * @param QString msg (optional) Message associated with the abortion
 */
void MeeboConnect::abort(QString msg)
{
    // Tell all remaining processes to stop
    //  (Implementation removed. The suicide variable is not used.)
    suicide = true;

    if (!msg.isEmpty())
    {
        emit error(msg);
    }
    else
    {
        emit error("Failed to download chat logs");
    }
}

/**
 * Getter: Pointer to Revision Number
 * @returns int* Revision number
 */
int* MeeboConnect::getRevision()
{
    return &revision;
}

/**
 * Getter: Pointer to Contacts List
 * @returns QList<QMap<QString, QVariant> >* Contacts list
 */
QList<QMap<QString, QVariant> >* MeeboConnect::getContacts()
{
    return &contacts;
}

/**
 * Setter: Pass Progress UI
 */
void MeeboConnect::passProgressProto(int meter, QString description)
{
    emit updateProgress(meter, description);
}

/**
 * Process "From" Request
 */
StdFormat* MeeboConnect::from(QHash<QString, QVariant> data)
{
    // Step 0/3: Setup.
    //  When buddies have been acquired, start downloading chat logs from them.
    connect(this, SIGNAL(updateAPIStatusBuddies()), SLOT(startDownloadingChatLogs()));

    // Step 1/3: Fetch the data.
    username = data["username"].toString();
    password = data["password"].toString();
    //  Authentication
    this->initialize(username, password);

    //  Wait for the chat logs to finish downloading...
    waiter = new QEventLoop(this);
    waiter->exec();

    //  Bail out of Meebo
    if (updater)
        updater->exit();
    if (downloader)
        downloader->exit();
    updateProgress(25, "Signing off...");
    this->signOffAPI();

    // Step 2/3: Process the data through the format converter "Meebo".
    //           (This class extends the "Meebo" class.)
    for (int i = 0; i < contacts.size(); i ++)
    {
        QMap<QString, QVariant> contact = contacts[i];

        this->setAccount(contact.value("account_assoc").toString());
        this->setProtocol(contact.value("protocol").toString());
        this->setWith(contact.value("username").toString());

        if (!contact.value("rawlog").toString().isEmpty())
            this->load(contact.value("rawlog"));
        updateProgress((25 * (i+1) / contacts.size()) + 25, "Interpreted " + QVariant(i+1).toString() + "/" + QVariant(contacts.size()).toString() + " logs...");
    }

    // Step 3/3: Submit the Log2Log-standardized chat log array.
    emit finished();
    return this->final;
}
