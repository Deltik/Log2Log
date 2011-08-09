/** @author igordcard */
/** This class is set for deprecation */
#ifndef HELPERS_H
#define HELPERS_H

#include <QDialog>
#include <QLayoutItem>

class Helpers
{
public:
    Helpers();

public:
    static void hideWebItems(QLayout *ql1, QLayout *ql2);
    static void hidePathItems(QLayout *ql, QWidget *qw);
    static void showWebItems(QLayout *ql1, QLayout *ql2);
    static void showPathItems(QLayout *ql, QWidget *qw);
};

#endif // HELPERS_H
