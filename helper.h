/**
 * @author igordcard
 * @author Deltik
 */
#ifndef HELPERS_H
#define HELPERS_H

#include <QDialog>
#include <QLayoutItem>
#include <QGridLayout>
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
    QString whatSpecificity(int index);
    QString whatTrinary(int bit);
};

#endif // HELPERS_H
