#include "conversion.h"
#include "ui_conversion.h"
#include <QTimer>

QTimer *timer; // Mock

Conversion::Conversion(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Conversion)
{
    ui->setupUi(this);
    timer = new QTimer;
    timer->setInterval(50);
    connect(timer, SIGNAL(timeout()), this, SLOT(progressMock()));
    timer->start();
}

Conversion::~Conversion()
{
    delete ui;
}

/**
 * Cancel Button
 * TODO
 */
void Conversion::on_xButton_clicked()
{
    this->close();
}

void Conversion::progressMock()
{
    ui->progressBar->setValue(ui->progressBar->value()+1);
    if(ui->progressBar->value() >= 100)
        timer->stop();
}
