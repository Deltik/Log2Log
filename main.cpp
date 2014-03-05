/**
 * Log2Log Chat Log Converter
 *  Execution
 *   Main File
 *
 * @author Deltik
 * @author igordcard
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

#include <QApplication>
#include <QtCore>
#include "log2log.h"
#include <iostream>
#include "update.h"
#include "conversionconsole.h"

#define N + "/n" +
#define println(a) cout << a << "\n"

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Command Line Processor
    QStringList cmdArgs = QCoreApplication::arguments();
    bool argsTriggered = false;
    if (cmdArgs.size() > 1)
        argsTriggered = true;
    for (int i = 0; i < cmdArgs.size(); i ++)
    {
        // Extract
        QString argument = cmdArgs[i];

        // ARGUMENT: Help
        if (argument == "-h" ||
            argument == "/h")
            argsTriggered = true;
        // ARGUMENT: Version
        if (argument == "-v")
        {
            string output = "Log2Log Chat Log Converter ";
            output += VERSION;

            println(output);

            return 0;
        }
        // ARGUMENT: License
        if (argument == "-l" ||
            argument == "--license")
        {
            cout << "Log2Log Chat Log Converter ";
            println(VERSION);
            println("");
            println("Log2Log is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.");
            println("");
            println("Log2Log is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.");
            println("");
            println("You should have received a copy of the GNU General Public License along with Log2Log.  If not, see <http://www.gnu.org/licenses/>.");
            println("");

            return 0;
        }
        // ARGUMENT: No Interface Mode
        if (argument == "-i" ||
            argument == "--no-interface")
        {
            ConversionConsole c(cmdArgs);

            return a.exec();
        }
    }
    if (argsTriggered)
    {
        println("Log2Log Chat Log Converter");
        println("");
        println("Usage:");
        println("  log2log [options] [-f FROM] [-t TO] [-s SOURCE] [-d DESTINATION]");
        println("");
        println("Help Options:");
        println("  -h, --help                     This help text");
        println("  -v, --version                  Show version information and exit");
        println("  -l, --license                  Show license information and exit");
        println("");
        println("Application Options:");
        println("  -i, --no-interface             Run without a user interface");
        println("  -f, --from                     `From` format converter name");
        println("  -t, --to                       `To` format converter name");
        println("  -s, --source                   Chat log source URI");
        println("  -d, --destination              Chat log destination URI");
        println("  -su, --source-username         Chat log source username");
        println("  -sp, --source-password         Chat log source password");
        println("  -du, --destination-username    Chat log destination username");
        println("  -dp, --destination-password    Chat log destination password");

        return 0;
    }

    // GUI Execution
    Log2Log w;

    w.show();

    return a.exec();
}
