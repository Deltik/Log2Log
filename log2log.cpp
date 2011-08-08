#include "log2log.h"
#include "helpers.h"
#include "ui_log2log.h"
#include "conversion.h"
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QMessageBox>

Log2Log::Log2Log(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Log2Log)
{
    ui->setupUi(this);
}

Log2Log::~Log2Log()
{
    delete ui;
}

void Log2Log::on_srcProtoBox_currentIndexChanged(int index)
{
    updateVisibleFields(index, 0);
}

void Log2Log::on_dstProtoBox_currentIndexChanged(int index)
{
    updateVisibleFields(index, 1);
}

void Log2Log::on_convertButton_clicked()
{
    Conversion *w = new Conversion(this);
    w->show();
    this->hide();
}

void Log2Log::updateVisibleFields(int arg1, int arg2)
{
    QLayout *userL;
    QLayout *passL;
    QLayout *pathL;
    QLabel *pathLb;

    if(arg2 == 0) {
        userL = ui->srcUserLayout;
        passL = ui->srcPassLayout;
        pathL = ui->srcPathLayout;
        pathLb = ui->srcPathLabel;
    }
    else {
        userL = ui->dstUserLayout;
        passL = ui->dstPassLayout;
        pathL = ui->dstPathLayout;
        pathLb = ui->dstPathLabel;
    }

    Helpers::showWebItems(userL, passL);
    Helpers::showPathItems(pathL, pathLb);

    switch(arg1) {
    case 0: Helpers::hideWebItems(userL, passL); break;
    case 1: Helpers::hideWebItems(userL, passL); break;
    case 2: Helpers::hideWebItems(userL, passL); break;
    case 3: Helpers::hidePathItems(pathL, pathLb); break;
    case 4: Helpers::hideWebItems(userL, passL); break;
    case 5: Helpers::hideWebItems(userL, passL); break;
    case 6: Helpers::hideWebItems(userL, passL); break;
    case 7: Helpers::hidePathItems(pathL, pathLb); break;
    case 8: Helpers::hidePathItems(pathL, pathLb); break;
    case 9: Helpers::hideWebItems(userL, passL); break;
    case 10: Helpers::hideWebItems(userL, passL); break;
    case 11: Helpers::hideWebItems(userL, passL); break;
    }
}
