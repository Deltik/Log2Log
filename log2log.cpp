#include "log2log.h"
#include "helper.h"
#include "ui_log2log.h"
#include "conversion.h"
#include "about.h"
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QFileDialog>
#include <QMessageBox>
#include <formatinfo.h>

Log2Log::Log2Log(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Log2Log)
{
    ui->setupUi(this);

    // Connect UI signals to local slots
    connect(ui->srcProtoBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateFields()));
    connect(ui->dstProtoBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateFields()));
    connect(ui->srcPathButton, SIGNAL(clicked()), this, SLOT(setSrcPath()));
    connect(ui->dstPathButton, SIGNAL(clicked()), this, SLOT(setDstPath()));
    connect(ui->convertButton, SIGNAL(clicked()), this, SLOT(startConversion()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(menuAbout()));

    updateFields();

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
        Helper::hide(ui->srcPathLayout);
        Helper::show(ui->srcAuthLayout);
    }
    else
    {
        Helper::show(ui->srcPathLayout);
        Helper::hide(ui->srcAuthLayout);
    }

    if (dstFI->getName("display").indexOf("(download)") >= 0)
    {
        Helper::hide(ui->dstPathLayout);
        Helper::show(ui->dstAuthLayout);
    }
    else
    {
        Helper::show(ui->dstPathLayout);
        Helper::hide(ui->dstAuthLayout);
    }

    /* Find Out What's Wrong With The Current Selections */
    whatsWrong();
}

/**
 * Folder Selection Dialog for Source
 */
void Log2Log::setSrcPath() {
    srcPath = QFileDialog::getExistingDirectory(this, tr("Source path"), QDir::currentPath());
    if(!srcPath.isNull())
        ui->srcPathEdit->setText(srcPath);
}

/**
 * Folder Selection Dialog for Destination
 */
void Log2Log::setDstPath() {
    dstPath = QFileDialog::getExistingDirectory(this, tr("Destination path"), QDir::currentPath());
    if(!dstPath.isNull())
        ui->dstPathEdit->setText(dstPath);
}

/**
 * Start Conversion Process
 */
void Log2Log::startConversion()
{
    QIcon srcIcon = ui->srcProtoBox->itemIcon(ui->srcProtoBox->currentIndex());
    QIcon dstIcon = ui->dstProtoBox->itemIcon(ui->dstProtoBox->currentIndex());
    Conversion *w = new Conversion(this, &srcIcon, &dstIcon);
    w->show();
}

/**
 * What's Lost in the Selected Conversion?
 */
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
    QString hGo    = "<span style=\"color: green;\">";
    QString hWarn  = "<li style=\"color: gold;\">";
    QString hError = "<li style=\"color: darkred;\">";
    QString hEnd   = "</li>";
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
        Helper* helper = new Helper();
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
        Helper* helper = new Helper();
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
               "</span>";

    // <frominfo> and <toinfo>
    if (!srcFI->getFromInfo().isEmpty() || !dstFI->getToInfo().isEmpty())
        html += "<hr />";
    if (!srcFI->getFromInfo().isEmpty())
        html += "<p>The " +
                srcFI->getName("display") +
                " source converter says: <code>" +
                srcFI->getFromInfo() +
                "</code></p>";
    if (!dstFI->getToInfo().isEmpty())
        html += "<p>The " +
                dstFI->getName("display") +
                " destination converter says: <code>" +
                dstFI->getToInfo() +
                "</code></p>";

    // So tell me: what's wrong?
    ui->whatsWrong->setHtml(html);
}

/**
 * About Log2Log Dialog Box
 */
void Log2Log::menuAbout()
{
    About *w = new About(this);
    w->show();
}
