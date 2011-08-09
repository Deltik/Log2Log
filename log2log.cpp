#include "log2log.h"
#include "helpers.h"
#include "ui_log2log.h"
#include "conversion.h"
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QFileDialog>
#include <QMessageBox>
#include <formatinfo.h>

/* DEBUG */
#include <xmltest.h>
#include <QDebug>

Log2Log::Log2Log(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Log2Log)
{
    ui->setupUi(this);

    // Connect UI signals to local slots
    //connect(ui->srcProtoBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSrcFields(int)));
    //connect(ui->dstProtoBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateDstFields(int)));
    /* NEW IMPLEMENTATION EXPERIMENTAL CODE - START */
    connect(ui->srcProtoBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateFields()));
    connect(ui->dstProtoBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateFields()));
    /* NEW IMPLEMENTATION EXPERIMENTAL CODE -  END  */
    connect(ui->srcPathButton, SIGNAL(clicked()), this, SLOT(setSrcPath()));
    connect(ui->dstPathButton, SIGNAL(clicked()), this, SLOT(setDstPath()));
    connect(ui->convertButton, SIGNAL(clicked()), this, SLOT(startConversion()));

    updateVisibleFields(ui->srcProtoBox->currentIndex(), 0);
    updateVisibleFields(ui->dstProtoBox->currentIndex(), 1);

    // Create "FormatInfo"
    FormatInfo* fi;

    // Clear Source Option Box
    ui->srcProtoBox->clear();
    // Fill Source Option Box
    fi = new FormatInfo();
    while (fi->pointerNext())
    {
        fi->pointerDig();
        // If applicable "from"
        if (fi->getFrom())
            ui->srcProtoBox->addItem(fi->getIcon(), fi->getName("display"));
    }

    // Clear Destination Option Box
    ui->dstProtoBox->clear();
    // Fill Destination Option Box
    fi = new FormatInfo();
    while (fi->pointerNext())
    {
        fi->pointerDig();
        // If applicable "to"
        if (fi->getTo())
            ui->dstProtoBox->addItem(fi->getIcon(), fi->getName("display"));
    }
}

Log2Log::~Log2Log()
{
    delete ui;
}

// Shows/hides "source" input boxes
void Log2Log::updateSrcFields(int index)
{
    updateVisibleFields(index, 0);
}

// Shows/hides "destination" input boxes
void Log2Log::updateDstFields(int index)
{
    updateVisibleFields(index, 1);
}

/**
 * Update Source and Destination Fields' Related Information
 */
void Log2Log::updateFields()
{
    // What is the current index of srcProtoBox?
    int srcIndex = ui->srcProtoBox->currentIndex();
    // What is the current index of dstProtoBox?
    int dstIndex = ui->dstProtoBox->currentIndex();

    // Load each indexs' profiles
    FormatInfo* srcFI = new FormatInfo();
    for (int i = 0; i <= srcIndex; i ++)
        srcFI->pointerNext();
    srcFI->pointerDig();
    FormatInfo* dstFI = new FormatInfo();
    for (int i = 0; i <= dstIndex; i ++)
        dstFI->pointerNext();
    dstFI->pointerDig();

    // Show the proper configuration fields
    if (srcFI->getName("display").indexOf("(download)") >= 0)
    {
        Helpers::showWebItems(ui->srcUserLayout, ui->srcPassLayout);
        Helpers::hidePathItems(ui->srcPathLayout, ui->srcPathLabel);
    }
    else
    {
        Helpers::hideWebItems(ui->srcUserLayout, ui->srcPassLayout);
        Helpers::showPathItems(ui->srcPathLayout, ui->srcPathLabel);
    }

    if (dstFI->getName("display").indexOf("(download)") >= 0)
    {
        Helpers::showWebItems(ui->dstUserLayout, ui->dstPassLayout);
        Helpers::hidePathItems(ui->dstPathLayout, ui->dstPathLabel);
    }
    else
    {
        Helpers::hideWebItems(ui->dstUserLayout, ui->dstPassLayout);
        Helpers::showPathItems(ui->dstPathLayout, ui->dstPathLabel);
    }
}

// Shows a folder selection dialog and stores the selected path, for source
void Log2Log::setSrcPath() {
    srcPath = QFileDialog::getExistingDirectory(this, tr("Source path"), QDir::currentPath());
    if(!srcPath.isNull())
        ui->srcPathEdit->setText(srcPath);
}

// Shows a folder selection dialog and stores the selected path, for destination
void Log2Log::setDstPath() {
    dstPath = QFileDialog::getExistingDirectory(this, tr("Destination path"), QDir::currentPath());
    if(!dstPath.isNull())
        ui->dstPathEdit->setText(dstPath);
}

// Initiates the conversion and switches to the proper dialog
void Log2Log::startConversion()
{
    QIcon srcIcon = ui->srcProtoBox->itemIcon(ui->srcProtoBox->currentIndex());
    QIcon dstIcon = ui->dstProtoBox->itemIcon(ui->dstProtoBox->currentIndex());
    Conversion *w = new Conversion(this, &srcIcon, &dstIcon);
    w->show();
    this->hide();
}

/**
 * Shows/hides input boxes depending on the protocol
 * @deprecated Don't use this anymore. Deltik has made a better method,
 *             Log2Log::updateFields().
 */
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

    // Shows everything before hiding something
    Helpers::showWebItems(userL, passL);
    Helpers::showPathItems(pathL, pathLb);

    switch(arg1) {
    case 0: Helpers::hideWebItems(userL, passL); break;
    case 1: Helpers::hideWebItems(userL, passL); break;
    case 2: Helpers::hideWebItems(userL, passL); break;
    case 3: Helpers::hideWebItems(userL, passL); break;
    case 4: Helpers::hidePathItems(pathL, pathLb); break;
    case 5: Helpers::hideWebItems(userL, passL); break;
    case 6: Helpers::hideWebItems(userL, passL); break;
    case 7: Helpers::hideWebItems(userL, passL); break;
    case 8: Helpers::hidePathItems(pathL, pathLb); break;
    case 9: Helpers::hidePathItems(pathL, pathLb); break;
    case 10: Helpers::hideWebItems(userL, passL); break;
    case 11: Helpers::hideWebItems(userL, passL); break;
    case 12: Helpers::hideWebItems(userL, passL); break;
    }
}

// DEBUG
void Log2Log::on_testButton_clicked()
{
    XmlTest *d = new XmlTest(this);
    d->show();
    this->hide();
}
