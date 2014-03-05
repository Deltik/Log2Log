/**
 * Log2Log Chat Log Converter
 *  Libraries
 *   API Handler
 *    Header File
 *
 * @author Deltik
 * @remarks Ported from PHP
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
 *
 */

#ifndef API_H
#define API_H

#include <QObject>
#include <QtNetwork>

class Api : public QThread
{
    Q_OBJECT
public:
    Api();
    void fetch();
    void setURL(QUrl $url);
    void setURL(QString $url);
    void addPost(QString index, QString data);
    void setHeader(QString key, QString value);
    QString getReply();
    QString getHeaders();
    // Reply (fetched by getURL())
    QByteArray replyData;
    QString str;
    QHash<QString, QString> hed;
    QNetworkCookieJar *jar;
    QMutex *lck;

protected:
    void run();

signals:
    void requestComplete(QString);
    void progress(qint64, qint64);
    void newSession();
    void finished();

public slots:
    void getURL();
    void replyFinished(QNetworkReply *);
    void readOn();
    void passProgress(qint64 bytesReceived, qint64 bytesTotal);
    void passError(QNetworkReply::NetworkError e);
    void cleanUp();

private:
    // Qt Network Access Manager
    QNetworkAccessManager *netHandler;
    // Qt Network Request (POST data)
    QNetworkRequest request;
    // Qt Network Reply
    QNetworkReply *reply;
    // URL (set by call to setURL())
    QUrl $url;
    // HTTP POST data
    QUrlQuery params;
    QByteArray $_POST;
};

#endif // API_H
