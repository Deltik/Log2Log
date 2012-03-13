/**
 * Log2Log Chat Log Converter
 *  Execution
 *   Main Window
 *    Header File
 *
 * @author igordcard
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

#ifndef LOG2LOG_H
#define LOG2LOG_H

#include <QtCore>
#include <QtGui>
#include "conversion.h"
#include "formats/stdconverter.h"

#include "formats/dependents/meeboconnectview.h"

// An object of each of these classes is needed in class Log2Log
class Conversion;
class StdConverter;

namespace Ui {
    class Log2Log;
}

class Log2Log : public QMainWindow
{
    Q_OBJECT

public:
    explicit Log2Log(QWidget *parent = 0);
    ~Log2Log();

protected:
    void updateVisibleFields(int arg1, int arg2);

public slots:
    Ui::Log2Log* getUi();
    void doGui(QString item, StdConverter *callback_object = NULL);

private slots:
    void quit();
    void menuUpdates();
    void menuAbout();
    void updateFields();
    void determineSrcDefault(QString newValue);
    void determineDstDefault(QString newValue);
    void setSrcPath();
    void setDstPath();
    void toggleConversion();
    void startConversion();
    void stopConversion();
    void setProgress(int meter, QString description);
    void handleConversionError(QString description);
    void whatsWrong(int srcIndex = NULL, int dstIndex = NULL);

private:
    // Variables: Main Interface
    Ui::Log2Log *ui;
    bool dontDefaultSrcPath;
    bool dontDefaultDstPath;
    QString srcPath;
    QString dstPath;
    // Variables: Conversion UI
    bool convertMode;
    QProgressBar progress;
    QLabel proginfo;
    bool error;
    // Variables: Conversion
    Conversion *cvHandler;
    // Variables: Other UI
    QList<QWidget*> widgets;
};

#endif // WIZARD_H
