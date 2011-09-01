/**
 * Log2Log Online Chat Log Converter
 *  Formats
 *   Omegle
 *
 * @author Deltik
 */

#include "omegle.h"
#include "helper.h"
#include <QtCore>

// QDebug
#include <QDebug>

/**
 * Constructor
 */
Omegle::Omegle()
{
}

/**
 * Destructor
 */
Omegle::~Omegle()
{
}

/**
 * Load a Chat Log
 */
void Omegle::load()
{
}

/**
 * Generate Log from Standardized Log
 */
void Omegle::generate()
{
}

/**
 * Process "From" Request
 */
StdFormat* Omegle::from(QHash<QString, QVariant> data)
{
    // Step 1/3: Fetch the data.
    QMap<QString, QVariant> list;
    if (data["files"].isNull())
        list = Helper::files_get_contents(data["path"].toString());
    else
        list = data["files"].toMap();
    QMap<QString, QVariant>::const_iterator i = list.constBegin();

    // Step 2/3: Process the data.
    /*foreach ($raw as $raw_item)
      {
      $this->load($raw_item);
      }*/
    int c = 0;
    while (i != list.constEnd())
    {
        qDebug()<<i.key();
        c++;
        msleep(1);
        updateProgress((40 * c / list.count()) + 10, "Interpreted " + QVariant(c).toString() + "/" + QVariant(list.count()).toString() + " files...");
        i ++;
    }

    // Step 3/3: Submit the Log2Log-standardized chat log array.
    //return $this->log;
    StdFormat* dummy = new StdFormat();
    return dummy;
}

/**
 * Process "To" Request
 */
void Omegle::to()
{
}
