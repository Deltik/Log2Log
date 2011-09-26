/**
 * Log2Log
 *  Formats
 *   Meebo (download)
 *    Header File
 *
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
#include "formats/stdconverter.h"
#include <QVariant>
#include <QNetworkAccessManager>

class MeeboConnect : public StdConverter
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
    virtual QString startAPI(QString bcookie = NULL);
    virtual QString updateAPI(qint32 rev = -1, QString sessionKey = NULL, QString clientId = NULL, qint32 focustime = -1);
    virtual void loginAPI(QString username, QString password, QString sessionKey = NULL, QString clientId = NULL);
    virtual void quitAPI(QString sessionKey = NULL, QString clientId = NULL);
    virtual void signOffAPI(QString sessionKey = NULL, QString locationId = NULL, QString clientId = NULL);
    virtual void mauserlistAPI(QString sessionKey = NULL, QString clientId = NULL, QString username = NULL);
    virtual void gwidAPI(QString sessionKey = NULL, QString clientId = NULL, QString username = NULL);
    virtual void dbgAPI(QString data, QString category = "javascript", QString sessionKey = NULL, QString clientId = NULL);
    virtual void infoAPI(QString username_with, QString username_self, MeeboConnect::Protocol, QString sessionKey = NULL, QString clientId = NULL);
    virtual QString getChatlogAPI(QString username_with, QString username_self, MeeboConnect::Protocol, QString username, QString sessionKey = NULL, QString clientId = NULL);
    virtual QStringList initialize(QString username, QString password, qint32 threshold = 5);
    virtual QStringList* parseContacts(QStringList data);
    virtual QStringList getAllChatlogs(QStringList buddies);
    virtual QStringList pullExternalSessionEvents(QStringList data);

public slots:
    virtual StdFormat* from(QHash<QString, QVariant> data);

private:
    virtual QString accessMCMD(QString func, QStringList params, QNetworkAccessManager::Operation op, bool https = false, bool mcmd = false);
    virtual QString accessMeebo(QString func, QStringList params, QNetworkAccessManager::Operation op, bool https = false);
    virtual QString accessAPI(QString command, bool https = false, QString context = NULL);
    void setAccount(QString account);
    void setProtocol(QString protocol);
    void setWith(QString with);

private:
    int total;
    QString $account;
    QString $protocol;
    QString $with;
};

#endif // MEEBOC_H
