/**
 * Log2Log Chat Log Converter
 *  Libraries
 *   General Helper
 *    Header File
 *
 * @author igordcard
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
#ifndef HELPERS_H
#define HELPERS_H

#include <QtCore>
#include "log2log.h"

class Helper
{
public:
    Helper();

public:
    static void hide(QGridLayout *ql);
    static void hide(QVBoxLayout *ql);
    static void show(QGridLayout *ql);
    static void show(QVBoxLayout *ql);
    static void convertMode(Ui::Log2Log *parent);
    static void mainMode(Ui::Log2Log *parent);
    static void hideWebItems(QLayout *ql1, QLayout *ql2);
    static void hidePathItems(QLayout *ql, QWidget *qw);
    static void showWebItems(QLayout *ql1, QLayout *ql2);
    static void showPathItems(QLayout *ql, QWidget *qw);
    QString whatPrecision(int index);
    QString whatTrinary(int bit);
    /* Log2Log Generic Chat Log Format Methods */
    static void postprocessor(StdFormat* $log);
    /* Custom PHP-based Functions */
    static QMap<QString, QVariant> files_get_contents(QString directory_path);
    static QList<QMap<QString, QVariant> > timezone_abbreviations_list();
    static QMap<QString, QVariant> zone_search(QVariant query);
    static QString stripslashes(QString original);

private:
    /* Custom PHP-based Functions */
    static void php_stripslashes(char *str, int *len);
};

#endif // HELPERS_H
