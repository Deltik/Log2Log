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
    QString getReply();
    // Reply (fetched by getURL())
    QString str;

protected:
    void run();

signals:
    void requestComplete(QString);

public slots:
    void getURL();
    void replyFinished(QNetworkReply *);

private:
    // Qt Network Access Manager
    QNetworkAccessManager* netHandler;
    // Qt Network Request (POST data)
    QNetworkRequest request;
    // URL (set by call to setURL())
    QUrl $url;
    // HTTP POST data
    QUrl params;
    QByteArray $_POST;
};

#endif // API_H
