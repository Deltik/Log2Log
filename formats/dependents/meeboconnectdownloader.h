/**
 * Log2Log
 *  Formats
 *   Meebo (download)
 *    Dependent Downloader Class
 *     Header File
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
 */

#ifndef MEEBOCONNECTDOWNLOADER_H
#define MEEBOCONNECTDOWNLOADER_H

#include <QtCore>
#include "../meeboconnect.h"

// An object of this class is needed in MeeboConnect
class MeeboConnect;

class MeeboConnectDownloader : public QThread
{
    Q_OBJECT

public:
    explicit MeeboConnectDownloader(MeeboConnect *mommy);
    void run();
    
signals:
    void updateProgress(int meter, QString description);
    void chatLogsDownloaded();
    
public slots:
    void getAllChatLogs();

private:
    MeeboConnect* parent;
    QList<QMap<QString, QVariant> >* contacts;
    
};

#endif // MEEBOCONNECTDOWNLOADER_H
