/**
 * Log2Log
 *  Formats
 *   Standard Converter
 *
 * @author Deltik
 * @remarks This class shows the persistent structure of all standard converter
 *          methods.
 *          The functions should be overloaded.
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
QVariant StdConverter::generate(StdFormat *$log)
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
void StdConverter::setInput(StdFormat *input)
{
    final = input;
}

/**
 * Get Output Data
 */
QVariant StdConverter::getData(QVariant)
{
    return data;
}
StdFormat* StdConverter::getData(StdFormat*)
{
    return final;
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
void StdConverter::to(StdFormat* $log)
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
        to(final);

    exec();
}
