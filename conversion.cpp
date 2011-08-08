#include "conversion.h"
#include "ui_conversion.h"
#include <QTimer>

QTimer *timer; // Mock
QWidget *p;

Conversion::Conversion(QWidget *parent, QIcon *srcIcon, QIcon *dstIcon) :
    QDialog(parent),
    ui(new Ui::Conversion)
{
    ui->setupUi(this);
    p = parent;
    timer = new QTimer;
    timer->setInterval(50);
    connect(timer, SIGNAL(timeout()), this, SLOT(progressMock()));
    timer->start();
    ui->srcIconLabel->setPixmap(srcIcon->pixmap(64,64));
    ui->dstIconLabel->setPixmap(dstIcon->pixmap(64,64));
}

Conversion::~Conversion()
{
    delete ui;
}

/**
 * Cancel Button
 */
void Conversion::on_xButton_clicked()
{
    p->show();
    this->close();
}

void Conversion::progressMock()
{
    ui->progressBar->setValue(ui->progressBar->value()+1);
    if(ui->progressBar->value() >= 100)
        timer->stop();
}
