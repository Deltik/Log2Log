#include "helper.h"
#include <QtGui/QLayout>
#include <QGridLayout>
#include "log2log.h"
#include "ui_log2log.h"

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

/**
 * Switch to Convert Mode
 * @param QWidget parent The MainWindow to edit
 */
void Helper::convertMode(Ui::Log2Log *parent)
{
    parent->srcProtoBox->setEnabled(false);
    parent->dstProtoBox->setEnabled(false);
    parent->srcPathEdit->setEnabled(false);
    parent->dstPathEdit->setEnabled(false);
    parent->srcPathButton->setEnabled(false);
    parent->dstPathButton->setEnabled(false);
    parent->srcUserEdit->setEnabled(false);
    parent->dstUserEdit->setEnabled(false);
    parent->srcPassEdit->setEnabled(false);
    parent->dstPassEdit->setEnabled(false);

    parent->convertButton->setIcon(QIcon());
    parent->convertButton->setIcon(QIcon::fromTheme("process-stop", QIcon(":/images/etc/cancel.png")));
}

/**
 * Switch to Main Mode
 * @param QWidget parent The MainWindow to edit
 */
void Helper::mainMode(Ui::Log2Log *parent)
{
    parent->srcProtoBox->setEnabled(true);
    parent->dstProtoBox->setEnabled(true);
    parent->srcPathEdit->setEnabled(true);
    parent->dstPathEdit->setEnabled(true);
    parent->srcPathButton->setEnabled(true);
    parent->dstPathButton->setEnabled(true);
    parent->srcUserEdit->setEnabled(true);
    parent->dstUserEdit->setEnabled(true);
    parent->srcPassEdit->setEnabled(true);
    parent->dstPassEdit->setEnabled(true);

    parent->convertButton->setIcon(QIcon::fromTheme("process-start", QIcon(":/images/etc/Arrow.png")));
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
