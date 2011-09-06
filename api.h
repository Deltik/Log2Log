/**
 * Log2Log
 *  Classes
 *   API Handler
 *    Header File
 *
 * @author Deltik
 * @remarks Ported from PHP
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
    QString getReply();

protected:
    void run();

signals:
    void requestComplete(QString);

public slots:
    void getURL();
    void replyFinished(QNetworkReply *);

private:
    QNetworkAccessManager* netHandler;
    QUrl $url;
    QString str;
};

#endif // API_H
