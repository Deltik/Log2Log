#include "helpers.h"
#include <QtGui/QLayout>

/* This class assumes and requires the existence
 * of log2log.ui which comes with Log2Log */

Helpers::Helpers()
{
}

// Hides input widgets related to web accounts data
void Helpers::hideWebItems(QLayout *ql1, QLayout *ql2)
{
    ql1->itemAt(0)->widget()->hide();
    ql1->itemAt(1)->widget()->hide();
    ql2->itemAt(0)->widget()->hide();
    ql2->itemAt(1)->widget()->hide();
}

// Hides input widgets related to the path of chatlogs
void Helpers::hidePathItems(QLayout *ql, QWidget *qw)
{
    ql->itemAt(0)->widget()->hide();
    ql->itemAt(1)->widget()->hide();
    qw->hide();
}

// Shows input widgets related to web accounts data
void Helpers::showWebItems(QLayout *ql1, QLayout *ql2)
{
    ql1->itemAt(0)->widget()->show();
    ql1->itemAt(1)->widget()->show();
    ql2->itemAt(0)->widget()->show();
    ql2->itemAt(1)->widget()->show();
}

// Shows input widgets related to the path of chatlogs
void Helpers::showPathItems(QLayout *ql, QWidget *qw)
{
    ql->itemAt(0)->widget()->show();
    ql->itemAt(1)->widget()->show();
    qw->show();
}
