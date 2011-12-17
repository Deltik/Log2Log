/**
 * Log2Log
 *  Formats
 *   Meebo (download)
 *    Dependent Session Keep-Alive Class
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

#include "meeboconnectkeepalive.h"

/**
 * Constructor
 */
MeeboConnectKeepAlive::MeeboConnectKeepAlive(MeeboConnect *mommy)
{
    // Link the parent locally.
    parent = mommy;
    // Create a QTimer object.
    updateCycler = new QTimer(this);
    // Create an API object.
    updateApi = new Api();
    // Store the revision number link.
    revision = parent->getRevision();
    // Move to thread
    moveToThread(this);
}

/**
 * Thread Runner
 */
void MeeboConnectKeepAlive::run()
{
    // Keep getting new Meebo "events"s...
    connect(updateCycler, SIGNAL(timeout()), this, SLOT(updateCycle()));
    updateCycler->start(0);

    // Enter the thread's event loop.
    exec();
}

/**
 * Capture a Meebo "events"
 */
void MeeboConnectKeepAlive::updateCycle()
{
    // This is where it gets difficult.
    // Capture data.

    // Trickery and deception
    if (*revision == 3)
    {
        parent->gwidAPI();
        parent->dbgAPI();
        parent->prefgetAPI();
    }
    // Get next update.
    QMap<QString, QVariant> temp = parent->updateAPI();
    QString result = temp.value("raw").toString();

    // Detect incorrect authentication.
    if (result.contains("\"protocol\":\"meebo\",\"data\":{\"type\":1,\"description\":\"Username does not exist\"}}}"))
    {
        emit updateAPIError("Incorrect username");
    }
    else if (result.contains("\"protocol\":\"meebo\",\"data\":{\"type\":2,\"description\":\"Incorrect password\"}}}"))
    {
        emit updateAPIError("Incorrect password");
    }
    else if (result.contains("\"protocol\":\"meebo\",\"data\":{\"type\":2,\"description\":\"Incorrect username or password.\"}}}"))
    {
        emit updateAPIError("Incorrect username or password");
    }
    else if (result.contains("\"data\":{\"stat\":\"fail\",\"msg\":\"Invalid request\",\"errorcode\":3}"))
    {
        emit updateAPIError("Bad login information");
    }

    // Detect update failures.
    else if (result.contains("<title>400 Bad Request</title>") ||
             result.contains("<title>502 Bad Gateway</title>"))
    {
        emit updateAPIError("Connection broke");
    }
    else if (temp.value("events").toList().size() == 0)
    {
        emit updateAPIError("Looks like you're IP-banned... :(");
    }

    // Broadcast the Meebo "events" data.
    emit updateAPIReply(temp);
}
