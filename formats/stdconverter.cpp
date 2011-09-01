/**
 * Log2Log
 *  Formats
 *   Standard Converter
 *
 * This class shows the persistent structure of all standard converter methods.
 * All of the functions should be overloaded.
 */

#include "stdconverter.h"

/**
 * Constructor
 */
StdConverter::StdConverter()
{
    moveToThread(this);
}

/**
 * Destructor
 */
StdConverter::~StdConverter()
{
}

/**
 * Load a Chat Log
 */
void StdConverter::load(QVariant $log_raw)
{
}

/**
 * Generate Log from Standardized Log
 */
void StdConverter::generate(StdFormat *$log)
{
}

/**
 * Delete All Work
 */
void StdConverter::unset()
{
    final = new StdFormat();
}

/**
 * Set "From/To" Mode
 */
void StdConverter::setMode(QString input)
{
    mode = input;
}

/**
 * Set Input Data
 */
void StdConverter::setInput(QVariant input)
{
    data = input;
}

/**
 * Process "From" Request
 */
StdFormat* StdConverter::from(QHash<QString, QVariant> data)
{
    return final;
}

/**
 * Process "To" Request
 */
void StdConverter::to()
{
}

/**
 * Thread Runner
 */
void StdConverter::run()
{
    if (mode == "from")
        from(data.toHash());
    else if (mode == "to")
        to(/*data*/);

    exec();
}
