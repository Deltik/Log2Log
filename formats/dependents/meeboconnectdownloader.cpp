/**
 * Log2Log
 *  Formats
 *   Meebo (download)
 *    Dependent Downloader Class
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

#include "meeboconnectdownloader.h"

MeeboConnectDownloader::MeeboConnectDownloader(MeeboConnect* mommy)
{
    // Link the parent locally.
    parent = mommy;
    // Link the parent MeeboConnect's contacts list locally.
    this->contacts = parent->getContacts();
    // Move to thread
    moveToThread(this);
}

void MeeboConnectDownloader::run()
{
    QTimer::singleShot(0, this, SLOT(getAllChatLogs()));

    exec();
}

void MeeboConnectDownloader::getAllChatLogs()
{
    for (int i = 0; i < contacts->size(); i ++)
    {qDebug()<<"CONTACTO NUMERO: "<<i+1<<" DE "<<contacts->size();
        // Make sure there was no terminate request.
        if (!parent->isRunning())
        {
            parent->signOffAPI();
            return;
        }

        // Extract
        QMap<QString, QVariant> contact = contacts->at(i);
        QString alias = contact["alias"].toString();

        // Skip if current contact is already processed
        if (contact["done"].toBool())
            continue;

        // Default alias to username
        if (alias.trimmed().isEmpty())
            alias = contact["username"].toString();

        // Display progress
        emit updateProgress((25 * i / contacts->size()),
                            "Downloading chat log " +
                            QVariant(i+1).toString() +
                            "/" + QVariant(contacts->size()).toString() +
                            "... (" +
                            alias +
                            " on " +
                            contact["protocol"].toString() +
                            ")");

        // Get chat log with the contact
        contact["rawlog"] = parent->getChatLogAPI(contact["username"].toString(), contact["account_assoc"].toString(), contact["protocol"].toString());

        // Mark that this contact has already been processed.
        contact["done"]   = true;

        // Save downloaded chat log
        contacts->operator [](i) = contact;
    }

    emit chatLogsDownloaded();qDebug()<<"CONFIRMED COMPLETED DOWNLOADING!!!";
}
