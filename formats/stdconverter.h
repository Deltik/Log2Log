/**
 * Log2Log
 *  Formats
 *   Standard Converter
 *    Header File
 *
 * @author Deltik
 * @remarks This class shows the persistent structure of all standard converter
 *          methods.
 *          The functions should be overloaded.
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

#ifndef STDCONVERTER_H
#define STDCONVERTER_H

#include "formats/stdformat.h"
#include "log2log.h"
#include <QtGui>
#include <QtCore>

// An object of each of these classes is needed in StdConverter
class Log2Log;

class StdConverter : public QThread
{
    Q_OBJECT

public:
    /* FUNCTIONS */
    StdConverter();
    ~StdConverter();
    virtual void load(QVariant $log_raw);
    virtual QVariant generate(StdFormat* $log);
    virtual void unset();
    void deleteLog() { this->unset(); }
    void remove() { this->unset(); }
    void destroy() { this->unset(); }
    virtual void setMode(QString input);
    virtual void setInput(QVariant input);
    virtual void setInput(StdFormat* input);
    virtual QVariant getData(QVariant);
    virtual StdFormat* getData(StdFormat*);
    virtual void run();
    /* VARIABLES */
    Log2Log *boss;
    StdFormat* final;
    QString mode;
    QVariant data;
    /* ERROR HANDLER DATA */
    QString errorText;
    QString warningText;
    QString noticeText;

public slots:
    /* FUNCTIONS */
    virtual StdFormat* from(QHash<QString, QVariant> data);
    virtual StdFormat* from();
    virtual void to(StdFormat* $log);
    virtual void to();
    virtual void setBoss(Log2Log *grandma);

signals:
    void updateProgress(int meter, QString description);
    void updateGui(QHash<QString, QVariant> doGuiParameters);
    void error(QString text = "");
    void warning(QString text = "");
    void notice(QString text = "");
};

#endif // STDCONVERTER_H
