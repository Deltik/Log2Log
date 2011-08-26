#include "helper.h"
#include <QtGui/QLayout>
#include <QGridLayout>

/* This class assumes and requires the existence
 * of log2log.ui which comes with Log2Log */

Helper::Helper()
{
}

/**
 * Bed Intruder: Hide Yo Kids: QGridLayout Edition
 */
void Helper::hide(QGridLayout *ql)
{
    for (int i = 0; i < ql->count(); i ++)
        ql->itemAt(i)->widget()->hide();
}

/**
 * Bed Intruder: Hide Yo Kids: QVBoxLayout Edition, namely the "PathLayout"s
 */
void Helper::hide(QVBoxLayout *ql)
{
    ql->itemAt(0)->widget()->hide();
    ql->itemAt(1)->layout()->itemAt(0)->widget()->hide();
    ql->itemAt(1)->layout()->itemAt(1)->widget()->hide();
}

/**
 * Show Items
 */
void Helper::show(QGridLayout *ql)
{
    for (int i = 0; i < ql->count(); i ++)
        ql->itemAt(i)->widget()->show();
}

/**
 * Show Items, namely the "PathLayout"s
 */
void Helper::show(QVBoxLayout *ql)
{
    ql->itemAt(0)->widget()->show();
    ql->itemAt(1)->layout()->itemAt(0)->widget()->show();
    ql->itemAt(1)->layout()->itemAt(1)->widget()->show();
}

// Hides input widgets related to web accounts data
void Helper::hideWebItems(QLayout *ql1, QLayout *ql2)
{
    ql1->itemAt(0)->widget()->hide();
    ql1->itemAt(1)->widget()->hide();
    ql2->itemAt(0)->widget()->hide();
    ql2->itemAt(1)->widget()->hide();
}

// Hides input widgets related to the path of chatlogs
void Helper::hidePathItems(QLayout *ql, QWidget *qw)
{
    ql->itemAt(0)->widget()->hide();
    ql->itemAt(1)->widget()->hide();
    qw->hide();
}

// Shows input widgets related to web accounts data
void Helper::showWebItems(QLayout *ql1, QLayout *ql2)
{
    ql1->itemAt(0)->widget()->show();
    ql1->itemAt(1)->widget()->show();
    ql2->itemAt(0)->widget()->show();
    ql2->itemAt(1)->widget()->show();
}

// Shows input widgets related to the path of chatlogs
void Helper::showPathItems(QLayout *ql, QWidget *qw)
{
    ql->itemAt(0)->widget()->show();
    ql->itemAt(1)->widget()->show();
    qw->show();
}

/**
 * Log2Log Timestamp Specificity Index Interpreter
 * @author Deltik
 */
QString Helper::whatSpecificity(int index)
{
    switch (index)
    {
    case -3: return "nanosecond";
    case -2: return "microsecond";
    case -1: return "millisecond";
    case 0: return "second";
    case 1: return "tenth second";
    case 2: return "minute";
    case 3: return "tenth minute";
    case 4: return "hour";
    case 5: return "tenth hour";
    case 6: return "day";
    case 7: return "week of a date";
    case 8: return "month of a date";
    case 9: return "year of a date";
    case 10: return "decade of a date";
    case 11: return "century of a date";
    case 12: return "millenium of a date";
    default: return "never. Time isn't supported by this";
    }
}

/**
 * Log2Log Trinary to String Converter
 * @author Deltik
 */
QString Helper::whatTrinary(int bit)
{
    switch (bit)
    {
    case 0: return "not stored at all";
    case 1: return "stored in groups of messages";
    case 2: return "stored with each message";
    default: return "not stored at all";
    }
}
