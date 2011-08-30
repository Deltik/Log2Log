/**
 * Log2Log Online Chat Log Converter
 *  Formats
 *   Omegle
 *
 * @author Deltik
 */

#include "omegle.h"
#include "helper.h"
#include <QMap>

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
    QMap<QString, QVariant> list = Helper::files_get_contents(data["from"].toString());

    // Step 2/3: Process the data.
    /*foreach ($raw as $raw_item)
      {
      $this->load($raw_item);
      }*/

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
