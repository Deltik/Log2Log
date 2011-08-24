/**
 * Log2Log Online Chat Log Converter
 *  Formats
 *   Omegle
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

#ifndef OMEGLE_H
#define OMEGLE_H

#include <QVariant>

class Omegle
{
/* FUNCTIONS */
public:
    Omegle();
    ~Omegle();
    QVariant load();
    QVariant generate();
    void unset();
    void deleteLog() { this->unset(); }
    void remove() { this->unset(); }
    void destroy() { this->unset(); }
    QVariant from();
    QVariant to();
};

#endif // OMEGLE_H