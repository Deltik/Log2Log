#include "update.h"
#include "ui_update.h"

Update::Update(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Update)
{
    ui->setupUi(this);
    connect(ui->backButton, SIGNAL(clicked()), this, SLOT(goBack()));
}

Update::~Update()
{
    delete ui;
}

/**
 * Go back / Cancel button
 */
void Update::goBack()
{
    this->close();
}
