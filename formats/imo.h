/**
 * Log2Log
 *  Formats
 *   imo.im (files)
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
 */

#ifndef IMO_H
#define IMO_H

#include "formats/stdformat.h"
#include "formats/stdconverter.h"
#include "helper.h"
#include <QtCore>
#include <QtXml>
#include <QDateTime>

class Imo : public StdConverter
{
    Q_OBJECT

public:
    Imo();
    virtual void load(QVariant $log_raw);
    virtual QVariant generate(StdFormat* $log);
    void setAccount(QString account);
    void setProtocol(QString protocol);
    void setWith(QString with);

public slots:
    virtual StdFormat* from(QHash<QString, QVariant> data);
    virtual void to(StdFormat* $log);

private:
    int total;
    QString $account;
    QString $protocol;
    QString $with;
};

#endif // IMO_H
