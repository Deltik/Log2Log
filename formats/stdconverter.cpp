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
void StdConverter::load()
{
}

/**
 * Generate Log from Standardized Log
 */
void StdConverter::generate()
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
