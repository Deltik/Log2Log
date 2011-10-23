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
#include "api.h"
#include "json.h"
#include <QVariant>
#include <QNetworkAccessManager>

class MeeboConnect : public Meebo
{
    Q_OBJECT

public:
    // TODO: build enum from XML
    enum Protocol {
        AIM,
        AujourdhuiCom,
        BasilMarket,
        Batanga,
        Bebo,
        CafeMom,
        ChessCom,
        DailyStrength,
        DanceTrippin,
        Decayenne,
        Dhingana,
        Digzibit,
        EBaumsWorld,
        ExperienceProject,
        Fabulously40,
        Fanpop,
        Flirtbox,
        Flixster,
        FootPets,
        Footytube,
        FunnelBrain,
        Fupa,
        GaiaOnline,
        Gamevance,
        GGL,
        GirlsAskGuys,
        GoogleTalk,
        Ibeatyou,
        ICQ,
        ImageChef,
        Indianelement,
        Jabber,
        JacketFlap,
        JamLegend,
        JustinTV,
        Klikot,
        MingleVille,
        MixPod,
        MuchGames,
        Multiply,
        MySpace,
        MyWorkster,
        MyYearbook,
        ObesityHelp,
        OperationSports,
        Outspark,
        Packed,
        PlayedOnline,
        Playfire,
        Playlist,
        PrisonBlock,
        Quepasa,
        SharedReviews,
        SocialSplash,
        Sonico,
        Sparkteens,
        Tehkseven,
        TrailBlazers,
        Ukstream,
        UltimateGuitar,
        UrFooz,
        Wadja,
        Wattpad,
        WeightLossBuddy,
        WindowsLive,
        Wixi,
        Xomba,
        Yahoo,
        Yonja,
        Zinch,
        Zorpia
    };

    MeeboConnect();
    virtual QMap<QString, QVariant> startAPI(QString bcookie = "");
    virtual QMap<QString, QVariant> updateAPI(qint32 rev = NULL, QString sessionKey = "", qint64 clientId = NULL, qint32 focustime = NULL);
    virtual void loginAPI(QString username, QString password, QString sessionKey = "", qint64 clientId = NULL);
    virtual void quitAPI(QString sessionKey = "", qint64 clientId = NULL);
    virtual void signOffAPI(QString sessionKey = "", qint64 locationId = NULL, qint64 clientId = NULL, bool clearSLT = false);
    virtual void mauserlistAPI(QString sessionKey = "", qint64 clientId = NULL, QString username_meebo = "");
    virtual void gwidAPI(QString sessionKey = "", qint64 clientId = NULL, QString username_meebo = "");
    virtual void dbgAPI(QString data = "wallpapers-unbranded=stock/null", QString category = "javascript", QString sessionKey = "", qint64 clientId = NULL);
    virtual void infoAPI(QString username_with, QString username_self, QString protocol, QString sessionKey = "", qint64 clientId = NULL);
    virtual QString getChatLogAPI(QString username_with, QString username_self, QString protocol, QString username_meebo = "", QString sessionKey = "", qint64 clientId = NULL);
    virtual void initialize(QString username, QString password, qint32 threshold = 10);
    virtual void parseContacts(QMap<QString, QVariant> data);
    virtual void getAllChatLogs();
    virtual QMap<QString, QVariant> pullExternalSessionEvents(QMap<QString, QVariant> data, bool &success);

public slots:
    virtual StdFormat* from(QHash<QString, QVariant> data);
    virtual void interpretReply(QString reply);
    void updateCycle();

private:
    virtual void accessCMD(QString func, QHash<QString, QString> params, QNetworkAccessManager::Operation op, bool https = false, bool mcmd = false);
    virtual void accessMeebo(QString func, QHash<QString, QString> params, QNetworkAccessManager::Operation op, bool https = false);
    virtual void accessAPI(QString command, bool https = false, QString context = "");

signals:
    void apiReply(QString);

private:
    // API Handler
    Api *api;
    QTimer *updateCycler;
    // API Response
    QString response;
    // Use TBD
    int total;
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
