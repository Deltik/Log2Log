/**
 * Log2Log Chat Log Converter
 *  Formats
 *   Standard Format
 *    Header File
 *
 * @author Deltik
 * @brief You can find out how Log2Log Generic Chat Log Format works by going
 *        into :/formats/stdformat.cpp .
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

#ifndef STDFORMAT_H
#define STDFORMAT_H

#include <QVariant>
#include <QDateTime>
#include <QTemporaryFile>
#include <QtSql/QSqlDatabase>

class StdFormat
{
/* FUNCTIONS */
public:
    StdFormat();
    ~StdFormat();
    /* Saver */
    void applyData();
    void applyEntry();
    void applyChat();
    void applyRow();
    void applySystem();
    void applyAuto();
    /* Extractor */
    void extractData();
    void extractEntry();
    void extractChat();
    void extractRow();
    void extractSystem();
    /* Browser */
    void resetPointer();
    int newEntry();
    bool hasNextEntry();
    bool nextEntry();
    bool hasPreviousEntry();
    bool previousEntry();
    bool gotoEntry(int index);
    bool toggleRow();
    int newRow();
    bool hasNextRow();
    bool nextRow();
    bool hasPreviousRow();
    bool previousRow();
    bool gotoRow(int index);
    bool switchLocalSystem(); // TODO: Implementation TBD
    bool switchSystem(); // TODO: Implementation TBD
    bool newSystem(); // TODO: Implementation TBD
    bool nextSystem(); // TODO: Implementation TBD
    bool previousSystem(); // TODO: Implementation TBD
    bool gotoSystem(); // TODO: Implementation TBD
    /* Setters */
    bool setClient(QString name);
    bool setProtocol(QString name);
    bool setSelf(QString username);
    bool setSelfAlias(QString myname);
    bool setWith(QString username);
    bool setWithAlias(QString buddyname);
    bool setTime(qlonglong time, bool forceBase = false);
    bool setTimezone(QString timezone, bool forceBase = false);
    bool setSender(QString name);
    bool setAlias(QString name);
    bool setContent(QString content);
    bool setCode(int code);
    bool setPrecision(int precision);
    bool setAccuracy(int accuracy);
    bool setNice(int nice);
    /* Getters */
    QString getVersion();
    QString getClient();
    QString getProtocol();
    QString getSelf();
    QString getSelfAlias();
    QString getWith();
    QString getWithAlias();
    qlonglong getTime();
    QString getTimezone();
    QString getSender();
    QString getAlias();
    QString getContent();
    int getCode();
    int getPrecision();
    int getAccuracy();
    int getNice();

/* VARIABLES */
public:
    // Final Construction

    /* On 07 June 2015, Deltik made the decision to replace the old QMap
    in-memory database with an in-disk SQLite 3 temporary database. This is
    much smarter than whatever Deltik was thinking way back in 2011 because
    chat logs are mappable to databases, and accessing databases is much more
    memory-friendly and probably faster than trying to write a QMap in-memory
    database system that could only seek forward and backward like a tape.

    The old format converter code is stuck with the tape concept, so there is
    an emulation layer in StdFormat now that makes the reader code compatible
    with a proper database. No more reinventing the wheel for Deltik... */

    QTemporaryFile db_file;
    QSqlDatabase db;
    QMap<QString, QVariant> log;
    QList<QVariant> data;
    QMap<QString, QVariant> entry;
    QList<QVariant> chat;
    QMap<QString, QVariant> row;
    QList<QVariant> system;

/* VARIABLES */
protected:
    QMap<QString, QVariant> empty;
    QList<QVariant> emptyl;

/* VARIABLES */
private:
    // Current Entry Index
    int dexEntry;
    // Current Chat Row
    int dexRow;
    // Whether in a Chat Row
    bool inRow;
    // Current System Log Row
    int dexSystem; // TODO: Implementation TBD

};

#endif // STDFORMAT_H
