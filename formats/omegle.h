/**
 * Log2Log
 *  Formats
 *   Omegle
 *    Header File
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

#ifndef OMEGLE_H
#define OMEGLE_H

#include "formats/stdformat.h"
#include "formats/stdconverter.h"
#include <QVariant>

class Omegle : public StdConverter
{
    Q_OBJECT

/* FUNCTIONS */
public:
    Omegle();
    virtual ~Omegle();
    virtual void load(QVariant $log_raw);
    virtual QVariant generate(StdFormat* $log);

public slots:
    virtual StdFormat* from(QHash<QString, QVariant> data);
    virtual void to(StdFormat* $log);

private:
    int total;
};

#endif // OMEGLE_H
