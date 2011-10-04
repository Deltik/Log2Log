/**
 * Log2Log
 *  Formats
 *   Meebo (download)
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
}

QString MeeboConnect::startAPI(QString bcookie)
{

}

QString MeeboConnect::updateAPI(qint32 rev, QString sessionKey, QString clientId, qint32 focustime)
{

}

void MeeboConnect::loginAPI(QString username, QString password, QString sessionKey, QString clientId)
{

}

void MeeboConnect::quitAPI(QString sessionKey, QString clientId)
{

}
void MeeboConnect::signOffAPI(QString sessionKey, QString locationId, QString clientId)
{

}

void MeeboConnect::mauserlistAPI(QString sessionKey, QString clientId, QString username)
{

}

void MeeboConnect::gwidAPI(QString sessionKey, QString clientId, QString username)
{

}

void MeeboConnect::dbgAPI(QString data, QString category, QString sessionKey, QString clientId)
{

}

void MeeboConnect::infoAPI(QString username_with, QString username_self, MeeboConnect::Protocol, QString sessionKey, QString clientId)
{

}

QString MeeboConnect::getChatLogAPI(QString username_with, QString username_self, MeeboConnect::Protocol, QString username, QString sessionKey, QString clientId)
{

}

QStringList MeeboConnect::initialize(QString username, QString password, qint32 threshold)
{

}

QStringList* MeeboConnect::parseContacts(QStringList data)
{

}

QStringList MeeboConnect::getAllChatLogs(QStringList buddies)
{

}

QStringList MeeboConnect::pullExternalSessionEvents(QStringList data)
{

}


/**
 * ##################################
 * # Meebo API Raw Access Functions #
 * ##################################
 */

/**
 * Top-level API accessor commands
 * @param QString func API's function
 * @param QMap<QString, QString> params Parameters to set in the API function
 * @param QNetworkAccessManager::Operation Operation of QNetworkAccessManager
 * @param bool https Set to TRUE for HTTPS access mode
 * @param bool mcmd Whether to use Meebo's MCMD or CMD API
 */
QString MeeboConnect::accessCMD(QString func, QMap<QString, QString> params, QNetworkAccessManager::Operation op, bool https, bool mcmd)
{
    if (mcmd)
        func += "mcmd/";
    else
        func += "cmd/";

    return this->accessMeebo(func, params, op, https);
}

/**
 * Accesses an JSON-API functions through this,
 * processes data for accessing the API,
 * accesses the API,
 * and returns the output.
 */
QString MeeboConnect::accessMeebo(QString func, QMap<QString, QString> params, QNetworkAccessManager::Operation op, bool https)
{
    if (op == QNetworkAccessManager::PostOperation)
    {
        QMapIterator<QString, QString> i(params);
        while (i.hasNext())
        {
            i.next();

            api.addPost(i.key(), i.value());
        }
    }
    else
    {
        QUrl get;
        QMapIterator<QString, QString> i(params);
        while (i.hasNext())
        {
            i.next();
            get.addQueryItem(i.key(), i.value());
        }
        func += "?" + get.encodedQuery();
    }
}

/**
 * Accesses the Meebo API directly.
 */
QString MeeboConnect::accessAPI(QString command, bool https, QString context)
{
    QString url;
    QString s = "";
    if (https == true)
        s = "s";
    url = "http"+s+"://www.meebo.com/" + command;
    api.setURL(url);
}

/**
 * MEEBO_CUSTOM: Set Account
 */
void MeeboConnect::setAccount(QString account)
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
void MeeboConnect::setProtocol(QString protocol)
{
    $protocol = protocol;
}

/**
 * MEEBO_CUSTOM: Set Other User's Account
 */
void MeeboConnect::setWith(QString with)
{
    $with = with;
}

/**
 * Process "From" Request
 */
StdFormat* MeeboConnect::from(QHash<QString, QVariant> data)
{
}
