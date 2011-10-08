/**
 * Log2Log Chat Log Converter
 *  Execution
 *   Format Converter Handler
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

#ifndef CONVERSION_H
#define CONVERSION_H

#include "ui_log2log.h"
#include "formats/stdformat.h"
#include "formats/stdconverter.h"
#include <QtCore>

class Conversion : public QThread
{
    Q_OBJECT

public:
    Conversion(QHash<QString, QVariant> import);
    Conversion(Ui::Log2Log *ui);
    void terminate_all();

protected:
    void run();

public slots:
    void collectData();
    void convertFrom();
    void convertTo();
    void save();
    void setProgressProto(int meter, QString description);
    void error(QString text);

private:
    // Variables: Main UI
    Ui::Log2Log *ui;
    // Variables: Data Collection
    QString from_name;
    QString to_name;
    QHash<QString, QVariant> from;
    QHash<QString, QVariant> to;
    // Variables: Format Converter Objects
    StdConverter* $FROM;
    StdConverter* $TO;
    // Variables: Chat Log
    StdFormat* final;
    // Functions
    QMap<QString, QVariant> files_get_contents(QString directory_path);

signals:
    void done();
    void updateProgress(int meter, QString description);
};

#endif // CONVERSION_H
