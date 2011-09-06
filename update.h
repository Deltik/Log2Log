#define VERSION "1.0.0"
#define DL_SOURCE "http://log2log.sourceforge.net/update/"

#ifndef UPDATE_H
#define UPDATE_H

#include <QDialog>
#include <QtCore>
#include <QtXml>
#include <QDesktopServices>

namespace Ui {
    class Update;
}

class Update : public QDialog
{
    Q_OBJECT

public:
    explicit Update(QWidget *parent = 0);
    ~Update();

private slots:
    void goBack();
    void loadChangelog();
    void displayChangelog(QString reply);
    void interpretReply(QString reply);
    void loadURL();
    void loadURL(QUrl url) { $url = url; loadURL(); }
    void loadURL(QString url) { loadURL(QUrl(url, QUrl::TolerantMode)); }

private:
    Ui::Update *ui;
    QHash<QString, QVariant> updateInfo;
    QUrl $url;
    bool dig_the_information_out(QXmlStreamReader &reader);
};

#endif // UPDATE_H

/* ############################### PHP Port ############################### */
QVariant version_compare(QString version1, QString version2, QString operater = NULL);
