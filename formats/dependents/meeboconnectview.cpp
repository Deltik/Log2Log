#include "meeboconnectview.h"
#include "ui_meeboconnectview.h"

MeeboConnectView::MeeboConnectView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MeeboConnectView)
{
    ui->setupUi(this);
}

MeeboConnectView::~MeeboConnectView()
{
    delete ui;
}
