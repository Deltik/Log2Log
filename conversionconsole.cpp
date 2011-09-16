/**
 * Log2Log Chat Log Converter
 *  Execution
 *   Non-Graphical Conversion
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

#include "conversionconsole.h"
#include <iostream>

using namespace std;

/**
 * Constructor
 */
ConversionConsole::ConversionConsole(QStringList argv)
{
    QHash<QString, QVariant> send;
    for (int i = 0; i < argv.size(); i ++)
    {
        // Extract
        QString a = argv[i];

        // FROM
        if (a == "-f" ||
            a == "/f" ||
            a == "--from")
            if (i+1 < argv.size())
                send["from"] = argv[i+1];

        // TO
        if (a == "-t" ||
            a == "/t" ||
            a == "--to")
            if (i+1 < argv.size())
                send["to"] = argv[i+1];

        // SOURCE
        if (a == "-s" ||
            a == "/s" ||
            a == "--source")
            if (i+1 < argv.size())
                send["source"] = argv[i+1];

        // SOURCE USERNAME
        if (a == "-su" ||
            a == "/su" ||
            a == "--source-username")
            if (i+1 < argv.size())
                send["source-username"] = argv[i+1];

        // SOURCE PASSWORD
        if (a == "-sp" ||
            a == "/sp" ||
            a == "--source-password")
            if (i+1 < argv.size())
                send["source-password"] = argv[i+1];

        // DESTINATION
        if (a == "-d" ||
            a == "/d" ||
            a == "--destination")
            if (i+1 < argv.size())
                send["destination"] = argv[i+1];

        // DESTINATION USERNAME
        if (a == "-du" ||
            a == "/du" ||
            a == "--destination-username")
            if (i+1 < argv.size())
                send["destination-username"] = argv[i+1];

        // DESTINATION PASSWORD
        if (a == "-dp" ||
            a == "/dp" ||
            a == "--destination-password")
            if (i+1 < argv.size())
                send["destination-password"] = argv[i+1];
    }

    /* ### GO!!! ### */
    cvHandler = new Conversion(send);
    connect(cvHandler, SIGNAL(updateProgress(int, QString)), this, SLOT(setProgress(int, QString)), Qt::QueuedConnection);
    connect(cvHandler, SIGNAL(finished()), this, SLOT(stopConversion()), Qt::QueuedConnection);

    cvHandler->start();
}

/**
 * Stop Conversion
 */
void ConversionConsole::stopConversion()
{
    /* STOP */
    cvHandler->terminate_all();
    cout << "\n";
    QApplication::quit();
}

/**
 * Set Progress UI
 */
void ConversionConsole::setProgress(int meter, QString description)
{
    cout << "\n" << QVariant(meter).toString().toStdString() << "% : " << description.toStdString();
}
