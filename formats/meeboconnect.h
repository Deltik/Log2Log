/**
 * Log2Log
 *  Formats
 *   Meebo (download)
 *    Header File
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

#ifndef MEEBOC_H
#define MEEBOC_H

#include "formats/stdformat.h"
#include "formats/meebo.h"
#include "formats/dependents/meeboconnectkeepalive.h"
#include "formats/dependents/meeboconnectdownloader.h"
#include "api.h"
#include "json.h"
#include <QVariant>
#include <QNetworkAccessManager>

// An object of each of these classes is needed in MeeboConnect
class MeeboConnectKeepAlive;
class MeeboConnectDownloader;

class MeeboConnect : public Meebo
{
    Q_OBJECT

public:
    /* CONSTRUCTOR */
    MeeboConnect();
    /* Meebo API Normal Access Functions */
    virtual QMap<QString, QVariant> startAPI(QString bcookie = "");
    virtual QMap<QString, QVariant> updateAPI(qint32 rev = NULL, QString sessionKey = "", qint64 clientId = NULL, qint32 focustime = NULL);
    virtual void loginAPI(QString username, QString password, QString sessionKey = "", qint64 clientId = NULL);
    virtual void quitAPI(QString sessionKey = "", qint64 clientId = NULL);
    virtual void signOffAPI(QString sessionKey = "", qint64 locationId = NULL, qint64 clientId = NULL, bool clearSLT = false);
    virtual void mauserlistAPI(QString sessionKey = "", qint64 clientId = NULL, QString username_meebo = "");
    virtual void gwidAPI(QString sessionKey = "", qint64 clientId = NULL, QString username_meebo = "");
    virtual void dbgAPI(QString data = "wallpapers-unbranded=stock/null", QString category = "javascript", QString sessionKey = "", qint64 clientId = NULL);
    virtual void prefgetAPI(QList<QVariant> items = QVariant().toList(), QString sessionKey = "", qint64 clientId = NULL);
    virtual void infoAPI(QString username_with, QString username_self, QString protocol, QString sessionKey = "", qint64 clientId = NULL);
    virtual QString getChatLogAPI(QString username_with, QString username_self, QString protocol, QString username_meebo = "", QString sessionKey = "", qint64 clientId = NULL);
    virtual QString getChatLogAlt(QString username_with, QString username_self, QString protocol, QString username_meebo = "", QString timePeriod = "", QString sessionKey = "");
    /* Meebo API Sophisticated Access Functions */
    virtual void initialize(QString username, QString password);
    virtual void parseContacts(QMap<QString, QVariant> data);
    virtual QMap<QString, QVariant> pullExternalSessionEvents(QMap<QString, QVariant> data, bool &success);

public slots:
    virtual StdFormat* from(QHash<QString, QVariant> data);
    /* Meebo API Raw Access Functions */
    virtual QString interpretReply(QString reply);
    /* Meebo API Normal Access Functions */
    virtual void updateCycle();
    /* Log2Log Functions */
    virtual void updateAPIHandler(QMap<QString, QVariant> data);
    virtual void startDownloadingChatLogs();
    virtual void getAllChatLogs();
    virtual void gotAllChatLogs();
    virtual void abort(QString msg = "");
    int* getRevision();
    QList<QMap<QString, QVariant> >* getContacts();
    void passProgressProto(int meter, QString description);

private:
    /* Meebo API Raw Access Functions */
    virtual QString accessCMD(QString func, QHash<QString, QString> params, QNetworkAccessManager::Operation op, bool https = false, bool mcmd = false, Api* apporter = NULL);
    virtual QString accessMeebo(QString func, QHash<QString, QString> params, QNetworkAccessManager::Operation op, bool https = false, Api* apporter = NULL);
    virtual QString accessAPI(QString command, bool https = false, Api* apporter = NULL);

signals:
    void apiReply(QString);
    void updateAPIReply(QMap<QString, QVariant>);
    void updateAPIError(QString);
    void updateAPIStatusAccounts();
    void updateAPIStatusBuddies();
    void updateAPIStatusConnected();
    void chatLogsDownloaded();

private:
    // API Handler
    Api *api;
    Api *updateApi;
    QTimer *updateCycler;
    // API Response
    QString response;
    // Threading Variables
    MeeboConnectKeepAlive *updater;
    MeeboConnectDownloader *downloader;
    QFuture<void> future;
    QFutureWatcher<void> watcher;
    QEventLoop *waiter;
    // Statistics Variables
    int total;
    bool chatLogsAreDownloadingAlready;
    bool suicide;
    // Session Variables
    QString sessionKey;
    qint64 sessionId;
    qint64 clientId;
    int revision;
    // Authentication Variables
    QString username;
    QString password;
    bool authenticated;
    // Storage Variables
    QMap<QString, QVariant> accounts;
    QList<QMap<QString, QVariant> > contacts;
};

#endif // MEEBOC_H
