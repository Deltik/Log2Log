#define VERSION "1.0.0"
#define DL_SOURCE "http://log2log.sourceforge.net/update/"

/**
 * Log2Log Chat Log Converter
 *  Execution
 *   Update Client
 *    Header File
 *
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
 *
 */

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
