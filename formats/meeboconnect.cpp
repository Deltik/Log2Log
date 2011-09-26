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
    final->setClient("MeeboConnect");
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
QString MeeboConnect::getChatlogAPI(QString username_with, QString username_self, MeeboConnect::Protocol, QString username, QString sessionKey, QString clientId)
{

}
QStringList MeeboConnect::initialize(QString username, QString password, qint32 threshold)
{

}
QStringList* MeeboConnect::parseContacts(QStringList data)
{

}
QStringList MeeboConnect::getAllChatlogs(QStringList buddies)
{

}
QStringList MeeboConnect::pullExternalSessionEvents(QStringList data)
{

}
QString MeeboConnect::accessMCMD(QString func, QStringList params, QNetworkAccessManager::Operation op, bool https, bool mcmd)
{

}
QString MeeboConnect::accessMeebo(QString func, QStringList params, QNetworkAccessManager::Operation op, bool https)
{

}
QString MeeboConnect::accessAPI(QString command, bool https, QString context)
{

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
