/**
 * Log2Log Online Chat Log Converter
 *  Formats
 *   Pidgin HTML
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

#ifndef PIDGIN_H
#define PIDGIN_H

#include "formats/stdformat.h"
#include "formats/stdconverter.h"
#include <QtCore>

class Pidgin : public StdConverter
{
    Q_OBJECT

/* FUNCTIONS */
public:
    Pidgin();
    virtual void load(QVariant $log_raw);
    virtual void loadHtml(QVariant $log_raw);
    virtual void loadPlainText(QVariant $log_raw);
    virtual void loadSystemHtml(QVariant $log_raw);
    virtual void loadSystemPlainText(QVariant $log_raw);
    virtual QVariant generate(StdFormat* $log);

public slots:
    virtual StdFormat* from(QHash<QString, QVariant> data);
    virtual void to(StdFormat* $log);

private:
    int total;
    qlonglong interpretTime(QString input, qlonglong $time_base = 0);
    QString readLine(QString text, qint64 line);
};

#endif // PIDGIN_H