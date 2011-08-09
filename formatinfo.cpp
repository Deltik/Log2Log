/**
 * Log2Log
 *  Classes
 *   Format Information
 */

#include "formatinfo.h"

/**
 * Constructor
 * @param QString unixname The format to look up
 */
FormatInfo::FormatInfo(char *unixname)
{
    // Open formats file
    QFile* file = new QFile(":/resources/formats.xml");
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }
    // Create XML stream reader from file
    QXmlStreamReader xml(file);

    // Read the XML file
    while(!xml.atEnd())
    {
        // Read next item
        QXmlStreamReader::TokenType token = xml.readNext();

        // Skip beginning of file
        if (token == QXmlStreamReader::StartDocument) continue;

        // If token is an element...
        if(token == QXmlStreamReader::StartElement)
        {
            // Bingo. If we found the format we're looking for...
            if (xml.attributes().value("id").toString() == unixname)
            {
                dig_the_information_out(xml);
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
 * Dig the Information Out
 * @precondition reader is currently pointing at a <format>
 * @param QXmlStreamReader reader The reader to pick up from
 * @returns void Nothing at all; it's saved into the class for getters
 */
void FormatInfo::dig_the_information_out(QXmlStreamReader &reader)
{
    // Check if reader is pointing at an opening <format> element
    if(reader.tokenType() != QXmlStreamReader::StartElement && reader.name() == "format") return;

    // Just one thing: Store the unixname
    fName_unix = reader.attributes().value("id").toString();

    // Okay, it's a <format>. Let's move on
    reader.readNext();

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
        if (currentElement == "icon")
        {
            fIcon_path = reader.text().toString();
            QIcon fIconn(fIcon_path);
            fIcon = fIconn;
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
        if (currentElement == "specificity")
        {
            fSpecificity = (int)reader.text().toString().toInt();
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
}
