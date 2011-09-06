/**
 * Log2Log
 *  Classes
 *   API Handler
 *
 * @author Deltik
 * @remarks Ported from PHP
 */

#include "api.h"

/**
 * Constructor
 */
Api::Api()
{
    netHandler = new QNetworkAccessManager(this);
    moveToThread(this);
}

/**
 * Thread Runner
 */
void Api::run()
{
    netHandler = new QNetworkAccessManager(this);
    connect(netHandler, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    getURL();

    exec();
}

/**
 * Set URL for use with QNetworkRequest
 * @param string $url Full URL of the API request
 */
void Api::setURL(QUrl url)
{
    $url = url;
}
/// Alias
void Api::setURL(QString url)
{
    return setURL(QUrl(url, QUrl::TolerantMode));
}

/**
 * Get URL using QNetworkRequest
 */
void Api::getURL()
{
    netHandler->get(QNetworkRequest($url));
}

void Api::replyFinished(QNetworkReply* pReply)
{
    QByteArray data = pReply->readAll();
    str = QVariant(data).toString();
    emit requestComplete(str);
}

QString Api::getReply()
{
    return str;
}
