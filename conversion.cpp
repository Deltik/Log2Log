#include "conversion.h"
#include "ui_conversion.h"
#include <QTimer>

QTimer *timer; // Mock

Conversion::Conversion(QWidget *parent, QIcon *srcIcon, QIcon *dstIcon) :
    QDialog(parent),
    ui(new Ui::Conversion)
{
    ui->setupUi(this);
    connect(ui->backButton, SIGNAL(clicked()), this, SLOT(goBack()));

    // Temporary timer mock
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
void Conversion::goBack()
{
    this->parentWidget()->show();
    timer->stop();
    this->close();
}

// Temporary timer mock
void Conversion::progressMock()
{
    ui->progressBar->setValue(ui->progressBar->value()+1);
    if(ui->progressBar->value() >= 100)
        timer->stop();
}
