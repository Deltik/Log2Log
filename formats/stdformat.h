/**
 * Log2Log Online Chat Log Converter
 *  Formats
 *   Standard Format
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

class StdFormat
{
/* FUNCTIONS */
public:
    StdFormat();
    ~StdFormat();
    /* Browser */
    void resetPointer();
    int newEntry();
    bool hasNextEntry();
    bool nextEntry();
    bool hasPreviousEntry();
    bool previousEntry();
    bool gotoEntry(int index);
    bool toggleLine();
    int newLine();
    bool hasNextLine();
    bool nextLine();
    bool hasPreviousLine();
    bool previousLine();
    bool gotoLine(int index);
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
    bool setTime(qlonglong time);
    bool setTimezone(QString timezone);
    bool setSender(QString name);
    bool setAlias(QString name);
    bool setContent(QString content);
    bool setCode(int code);
    bool setPrecision(int precision);
    bool setAccuracy(int accuracy);
    bool setNice(int nice);
    /* Getters */
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
    //  This is an unordered indexed array (QHash).
    //  The slower, ordered indexed array (QMap) is not necessary.
    //  The index is a string (QString).
    //  The associated value can be one of many things (QVariant).
    QHash<QString, QVariant> final;

/* VARIABLES */
private:
    // Current Entry Index
    int dexEntry;
    // Current Chat Row
    int dexLine;
    // Whether in a Chat Row
    bool inLine;
    // Current System Log Row
    int dexSystem; // TODO: Implementation TBD

};

#endif // STDFORMAT_H
