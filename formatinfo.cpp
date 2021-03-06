/**
 * Log2Log Chat Log Converter
 *  Libraries
 *   Format Information
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

#include "formatinfo.h"

/**
 * Constructor: No Parameters
 */
FormatInfo::FormatInfo()
{
    xmlsrc = ":/resources/formats.xml";
    // Open formats file
    QFile* file = new QFile(xmlsrc);
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }
    // Create XML stream reader from file
    QXmlStreamReader xml(file);
    num_of_readNexts = 0;
}

/**
 * Constructor: With Format To Search For
 * @param QString unixname The format to look up
 */
FormatInfo::FormatInfo(QString unixname)
{
    xmlsrc = ":/resources/formats.xml";
    // Open formats file
    QFile* file = new QFile(xmlsrc);
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }
    // Create XML stream reader from file
    QXmlStreamReader xml(file);
    num_of_readNexts = 0;

    // Read the XML file
    while(!xml.atEnd())
    {
        // Read next item
        QXmlStreamReader::TokenType token = xml.readNext(); num_of_readNexts ++;

        // Skip beginning of file
        if (token == QXmlStreamReader::StartDocument) continue;

        // If token is an element...
        if(token == QXmlStreamReader::StartElement)
        {
            // Bingo. If we found the format we're looking for...
            if (xml.attributes().value("id").toString() == unixname)
            {
                dig_the_information_out(xml);
                break;
            }
        }
    }
}

/**
 * Destructor
 */
FormatInfo::~FormatInfo()
{
}

/**
 * Browser: Reset Pointer
 */
void FormatInfo::pointerReset()
{
    FormatInfo();
}

/**
 * Browser: Point to Next Format
 * @returns bool true if there is a next to point at
 */
bool FormatInfo::pointerNext()
{
    // Open formats file
    QFile* file = new QFile(xmlsrc);
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }
    // Create XML stream reader from file
    QXmlStreamReader xml(file);
    QXmlStreamReader::TokenType token;
    // Catch up
    for (int i = 0; i < num_of_readNexts; i ++)
    {
        token = xml.readNext();
    }
    // Dare to go on
    token = xml.readNext(); num_of_readNexts ++;
    // Repeat until <format> found or reached the end
    while (!(token == QXmlStreamReader::StartElement && xml.name() == "format") && !xml.atEnd())
    {
        token = xml.readNext(); num_of_readNexts ++;
    }
    if (token == QXmlStreamReader::StartElement && xml.name() == "format")
    {
        return true;
    }
    // No more
    return false;
}

/**
 * Browser: Point to Previous Format
 * @returns bool true if there is a previous to point at
 */
bool FormatInfo::pointerPrevious()
{
    // TODO:
    //  This is not a critical feature, so FormatInfo can work fine without it.
    return false; // <-- That's a semicolon. :)
}

/**
 * Browser: Get Information from Current Pointer Location
 * @returns bool true if successful
 */
bool FormatInfo::pointerDig()
{
    // Open formats file
    QFile* file = new QFile(xmlsrc);
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }
    // Create XML stream reader from file
    QXmlStreamReader xml(file);
    QXmlStreamReader::TokenType token;
    // Catch up
    for (int i = 0; i < num_of_readNexts; i ++)
    {
        token = xml.readNext();
    }
    return dig_the_information_out(xml);
}

/**
 * Dig the Information Out
 * @precondition reader is currently pointing at a <format>
 * @param QXmlStreamReader reader The reader to pick up from
 * @returns bool true if successful
 */
bool FormatInfo::dig_the_information_out(QXmlStreamReader &reader)
{
    // Check if reader is pointing at an opening <format> element
    if(reader.tokenType() != QXmlStreamReader::StartElement && reader.name() == "format") return false;

    // Just one thing: Store the unixname
    fName_unix = reader.attributes().value("id").toString();

    // Okay, it's a <format>. Let's move on
    reader.readNext();

    // Clean Variables
    fName_display = "";
    fName_client = "";
    fName_class = "";
    fIcon_path = "";
    QIcon fIconn(":/images/etc/blank.png");
    fIcon = fIconn;
    fTo = NULL;
    fFrom = NULL;
    fPrecision = NULL;
    fAccuracy = NULL;
    fTime = NULL;
    fTimezone = NULL;
    fStatus = NULL;
    fSystem = NULL;
    fAliases = NULL;
    fGroup = NULL;
    fFromInfo = "";
    fToInfo = "";

    // Keep going until </format>
    while(!(reader.tokenType() == QXmlStreamReader::EndElement && reader.name() == "format"))
    {
        QString currentElement = "format";

        if (reader.tokenType() == QXmlStreamReader::StartElement)
        {
            currentElement = reader.name().toString();
            reader.readNext();
            if (reader.tokenType() != QXmlStreamReader::Characters)
            {
                continue;
            }
        }

        if (currentElement == "name")
        {
            fName_display = reader.text().toString();
        }
        if (currentElement == "client")
        {
            fName_client = reader.text().toString();
        }
        if (currentElement == "class")
        {
            fName_class = reader.text().toString();
        }
        if (currentElement == "icon")
        {
            fIcon_path = reader.text().toString();
            QIcon fIconn(fIcon_path);
            fIcon = fIconn;
        }
        if (currentElement == "default")
        {
            QString path_proc = reader.text().toString();
            path_proc = path_proc.replace("~", QDir::homePath());
            path_proc = path_proc.replace("%APPDATA%", QStandardPaths::displayName(QStandardPaths::DataLocation));
            path_proc = path_proc.replace("///", "/");
            path_proc = path_proc.replace("//", "/");
            fDefaultPath = QDir::toNativeSeparators(path_proc);
        }
        if (currentElement == "to")
        {
            if (reader.text() == "true")
                fTo = true;
            else
                fTo = false;
        }
        if (currentElement == "from")
        {
            if (reader.text() == "true")
                fFrom = true;
            else
                fFrom = false;
        }
        if (currentElement == "precision")
        {
            fPrecision = (int)reader.text().toString().toInt();
        }
        if (currentElement == "accuracy")
        {
            if (reader.text() == "true")
                fAccuracy = true;
            else
                fAccuracy = false;
        }
        if (currentElement == "time")
        {
            if (reader.text() == "yes") fTime = 2;
            else if (reader.text() == "group") fTime = 1;
            else fTime = 0;
        }
        if (currentElement == "timezone")
        {
            if (reader.text() == "yes") fTimezone = 2;
            else if (reader.text() == "group") fTimezone = 1;
            else fTimezone = 0;
        }
        if (currentElement == "status")
        {
            if (reader.text() == "true")
                fStatus = true;
            else
                fStatus = false;
        }
        if (currentElement == "system")
        {
            if (reader.text() == "true")
                fSystem = true;
            else
                fSystem = false;
        }
        if (currentElement == "aliases")
        {
            if (reader.text() == "true")
                fAliases = true;
            else
                fAliases = false;
        }
        if (currentElement == "group")
        {
            if (reader.text() == "true")
                fGroup = true;
            else
                fGroup = false;
        }
        if (currentElement == "frominfo")
        {
            fFromInfo = reader.text().toString();
        }
        if (currentElement == "toinfo")
        {
            fToInfo = reader.text().toString();
        }

        // Next!
        reader.readNext();
    }
    // Success or Failure
    if (!fName_unix.isEmpty())
        return true;
    return false;
}
