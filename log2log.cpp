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
#include <QDebug>

Log2Log::Log2Log(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Log2Log)
{
    ui->setupUi(this);

    // Connect UI signals to local slots
    connect(ui->srcProtoBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateFields()));
    connect(ui->dstProtoBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateFields()));
    connect(ui->srcPathButton, SIGNAL(clicked()), this, SLOT(setSrcPath()));
    connect(ui->dstPathButton, SIGNAL(clicked()), this, SLOT(setDstPath()));
    connect(ui->convertButton, SIGNAL(clicked()), this, SLOT(startConversion()));

    updateFields();
    this->setWindowIcon(QIcon(":/images/etc/favicon.ico"));

    /* Create "FormatInfo" */
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
            ui->srcProtoBox->addItem(fi->getIcon(), fi->getName("display"), QVariant(fi->getName("unix")));
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
            ui->dstProtoBox->addItem(fi->getIcon(), fi->getName("display"), QVariant(fi->getName("unix")));
    }

    /* Find Out What's Wrong With The Current Selections */
    whatsWrong();
}

Log2Log::~Log2Log()
{
    delete ui;
}

/**
 * Shows/hides "source" input boxes
 * @deprecated Don't use this anymore. Deltik has made a better method,
 *             Log2Log::updateFields().
 */
void Log2Log::updateSrcFields(int index)
{
    updateVisibleFields(index, 0);
}

/**
 * Shows/hides "destination" input boxes
 * @deprecated Don't use this anymore. Deltik has made a better method,
 *             Log2Log::updateFields().
 */
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
    FormatInfo* srcFI = new FormatInfo(ui->srcProtoBox->itemData(srcIndex).toString());
    srcFI->pointerDig();
    FormatInfo* dstFI = new FormatInfo(ui->dstProtoBox->itemData(dstIndex).toString());
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

    /* Find Out What's Wrong With The Current Selections */
    whatsWrong();
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

void Log2Log::whatsWrong(int srcIndex, int dstIndex)
{
    if (!srcIndex) srcIndex = ui->srcProtoBox->currentIndex();
    if (!dstIndex) dstIndex = ui->dstProtoBox->currentIndex();

    // Load each indexs' profiles
    FormatInfo* srcFI = new FormatInfo(ui->srcProtoBox->itemData(srcIndex).toString());
    srcFI->pointerDig();
    FormatInfo* dstFI = new FormatInfo(ui->dstProtoBox->itemData(dstIndex).toString());
    dstFI->pointerDig();

    // Find out what's lost in this conversion:
    QString html = "<ul>";
    QString hGo    = "<li style=\"color: green;\">";
    QString hWarn  = "<li style=\"color: gold;\">";
    QString hError = "<li style=\"color: darkred;\">";
    QString hEnd   = "</li>";
    int     cGo    = 0;
    int     cWarn  = 0;
    int     cError = 0;

    // Same format conversion warning
    if (srcFI->getName() == dstFI->getName())
    {
        html += hWarn+"It's pointless to convert to the same format."+hEnd;
        cWarn ++;
    }
    // No time supported in destination
    if (dstFI->getTime() < srcFI->getTime() && dstFI->getTime() == 0)
    {
        html += hError + "Cannot save time" + hEnd;
        cError ++;
    }
    // Accuracy Index not supported in destination
    if (!dstFI->getAccuracy() && srcFI->getAccuracy())
    {
        html += hError +
                "No Log2Log Message Content Accuracy Index support. " +
                "Messages that are not precisely character-for-character (e.g. voice transcripts) " +
                "will be assumed to be precise in " +
                dstFI->getName("display") +
                "." +
                hEnd;
        cError ++;
    }
    // Specificity Index not as precise in destination
    if (srcFI->getTime() > 0 && dstFI->getTime() > 0 && srcFI->getSpecificity() < dstFI->getSpecificity())
    {
        Helpers* helper = new Helpers();
        QString srcSpecificityReadable = helper->whatSpecificity(srcFI->getSpecificity());
        QString dstSpecificityReadable = helper->whatSpecificity(dstFI->getSpecificity());
        html += hError +
                "Times are not as accurately saved. " +
                srcFI->getName("display") +
                " can store as close as a <b>" +
                srcSpecificityReadable +
                "</b>, but " +
                dstFI->getName("display") +
                " can only store as close as a <b>" +
                dstSpecificityReadable +
                "</b>." +
                hEnd;
        cError ++;
    }
    // Timezone handling
    if (dstFI->getTime() > 0 && dstFI->getTimezone() < srcFI->getTimezone())
    {
        Helpers* helper = new Helpers();
        if (dstFI->getTimezone() == 1)
        {
            html += hWarn;
            cWarn ++;
        }
        else
        {
            html += hError;
            cError ++;
        }
        html += "In " +
                srcFI->getName("display") +
                ", timezones are <b>" +
                helper->whatTrinary(srcFI->getTimezone()) +
                "</b>, but in " +
                dstFI->getName("display") +
                ", timezones are <b>" +
                helper->whatTrinary(dstFI->getTimezone()) +
                "</b>.";
        if (dstFI->getTimezone() == 1)
            html += " Log2Log will split conversations to fit timezones together.";
        html += hEnd;
    }
    // Inline status changes
    if (!dstFI->getStatus() && srcFI->getStatus())
    {
        if (dstFI->getSystem())
        {
            html += hWarn;
            cWarn ++;
        }
        else
        {
            html += hError;
            cError ++;
        }
        html += srcFI->getName("display") +
                " supports saving status changes (e.g. your friend going invisible while you're chatting with her) in the conversation, but " +
                dstFI->getName("display") +
                " doesn't.";
        if (dstFI->getSystem())
        {
            html += " Status changes will be lost from the conversation, but Log2Log will save them in the system log.";
        }
        html += hEnd;
    }
    // System log
    if (!dstFI->getSystem() && srcFI->getSystem())
    {
        html += hError +
                "There is no system log. Log2Log cannot preserve any system log data that " +
                srcFI->getName("display") +
                " contains." +
                hEnd;
        cError ++;
    }
    // Alias support
    if (!dstFI->getAliases() && srcFI->getAliases())
    {
        html += hError +
                "Buddy aliases are not supported. (e.g. Cannot store \"John Smith\" for \"johnsmith@example.com\")" +
                hEnd;
        cError ++;
    }
    // Group chats
    if (!dstFI->getGroup() && srcFI->getGroup())
    {
        html += hError +
                "Group chats are not supported. Log2Log cannot convert any group chats to " +
                dstFI->getName("display") +
                "." +
                hEnd;
        cError ++;
    }

    // Closing
    html += "</ul>";
    if (cError > 0)
        html = "<b>You may lose information by converting from </b><span style=\"text-decoration: underline;\">" +
               srcFI->getName("display") +
               "</span><b> to </b><span style=\"text-decoration: underline;\">" +
               dstFI->getName("display") +
               "</span><b>. These are the problems with the destination format:</b>" +
               html;
    else if (cWarn > 0)
        html = "<b>Warnings:</b>" +
               html;
    else
        html = hGo +
               dstFI->getName("display") +
               " completely supports " +
               srcFI->getName("display") +
               ". All data should be transferred." +
               hEnd;

    // <frominfo> and <toinfo>
    if (!srcFI->getFromInfo().isEmpty() || !dstFI->getToInfo().isEmpty())
        html += "<hr />";
    if (!srcFI->getFromInfo().isEmpty())
        html += "<p>The " +
                srcFI->getName("display") +
                " converter says: <code>" +
                srcFI->getFromInfo() +
                "</code></p>";
    if (!dstFI->getToInfo().isEmpty())
        html += "<p>The " +
                dstFI->getName("display") +
                " converter says: <code>" +
                dstFI->getToInfo() +
                "</code></p>";

    // So tell me: what's wrong?
    ui->whatsWrong->setHtml(html);
}
