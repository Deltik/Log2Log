/**
 * Log2Log Chat Log Converter
 *  Execution
 *   Non-Graphical Conversion
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

#ifndef CONVERSIONCONSOLE_H
#define CONVERSIONCONSOLE_H

#include <QtCore>
#include "conversion.h"

class ConversionConsole : public QObject
{
    Q_OBJECT

public:
    ConversionConsole(QStringList argv);

signals:

public slots:
    void setProgress(int meter, QString description);
    void stopConversion();

private:
    // Variables: Conversion
    Conversion *cvHandler;
};

#endif // CONVERSIONCONSOLE_H
