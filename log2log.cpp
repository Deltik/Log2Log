/**
 * Log2Log Chat Log Converter
 *  Execution
 *   Main Window
 *
 * @author Deltik
 *
 * License:
 *  This file is part of Log2Log.
 *
 *  Log2Log is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Log2Log is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Log2Log.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "log2log.h"
#include "helper.h"
#include "ui_log2log.h"
#include "update.h"
#include "about.h"
#include "formatinfo.h"
#include "conversion.h"
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QFileDialog>
#include <QTimer>
#include <QMessageBox>

Log2Log::Log2Log(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Log2Log)
{
    ui->setupUi(this);

    // Connect UI signals to local slots
    //  Format converter select boxes
    connect(ui->srcProtoBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateFields()));
    connect(ui->dstProtoBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateFields()));
    //  Path input boxes
    connect(ui->srcPathEdit, SIGNAL(textEdited(QString)), this, SLOT(determineSrcDefault(QString)));
    connect(ui->dstPathEdit, SIGNAL(textEdited(QString)), this, SLOT(determineDstDefault(QString)));
    //  Path selection UI buttons
    connect(ui->srcPathButton, SIGNAL(clicked()), this, SLOT(setSrcPath()));
    connect(ui->dstPathButton, SIGNAL(clicked()), this, SLOT(setDstPath()));
    //  Enter key detection for path and authentication fields
    connect(ui->srcPathEdit, SIGNAL(returnPressed()), this, SLOT(startConversion()));
    connect(ui->dstPathEdit, SIGNAL(returnPressed()), this, SLOT(startConversion()));
    connect(ui->srcUserEdit, SIGNAL(returnPressed()), this, SLOT(startConversion()));
    connect(ui->srcPassEdit, SIGNAL(returnPressed()), this, SLOT(startConversion()));
    connect(ui->dstUserEdit, SIGNAL(returnPressed()), this, SLOT(startConversion()));
    connect(ui->dstPassEdit, SIGNAL(returnPressed()), this, SLOT(startConversion()));
    //  `Go` button
    connect(ui->convertButton, SIGNAL(clicked()), this, SLOT(toggleConversion()));
    //  Menu bar items
    connect(ui->actionUpdates, SIGNAL(triggered()), this, SLOT(menuUpdates()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(menuAbout()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(quit()));

    // Set Menu Icons
    ui->actionAbout->setIcon(QIcon::fromTheme("help-about"));
    ui->actionExit->setIcon(QIcon::fromTheme("application-exit"));
    ui->actionUpdates->setIcon(QIcon::fromTheme("system-software-update"));
    ui->convertButton->setIcon(QIcon::fromTheme("process-start", QIcon(":/images/etc/Arrow.png")));

    // Setup Convert UI
    convertMode = false;
    // Configure Conversion UI
    progress.setObjectName("progressBar");
    progress.setFixedSize(200, 16);
    progress.setValue(0);
    proginfo.setObjectName("progressInfo");
    proginfo.setStyleSheet("font-size: 12px;");
    // Place Progress UI
    ui->statusBar->addWidget(&progress);
    ui->statusBar->addWidget(&proginfo);
    // Hide Progress UI
    progress.hide();
    proginfo.hide();

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
    this->quit();
}

void Log2Log::quit()
{
    if (convertMode)
        toggleConversion();
    this->close();
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

    // Set default path, if applicable
    if (!dontDefaultSrcPath)
        ui->srcPathEdit->setText(srcFI->getDefaultPath());
    if (!dontDefaultDstPath)
        ui->dstPathEdit->setText(dstFI->getDefaultPath());

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

    if (dstFI->getName("display").indexOf("(upload)") >= 0)
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
 * Determine Whether srcPathEdit Should Be Defaulted
 */
void Log2Log::determineSrcDefault(QString newValue)
{
    dontDefaultSrcPath = true;
    if (newValue.isEmpty())
        dontDefaultSrcPath = false;
}

/**
 * Determine Whether dstPathEdit Should Be Defaulted
 */
void Log2Log::determineDstDefault(QString newValue)
{
    dontDefaultDstPath = true;
    if (newValue.isEmpty())
        dontDefaultDstPath = false;
}

/**
 * Folder Selection Dialog for Source
 */
void Log2Log::setSrcPath() {
    QString initPath = ui->srcPathEdit->text();
    if (initPath.isEmpty())
        initPath = QDir::homePath();
    srcPath = QFileDialog::getExistingDirectory(this, tr("Source path"), initPath);
    if(!srcPath.isNull())
    {
        ui->srcPathEdit->setText(srcPath);
        dontDefaultSrcPath = true;
    }
}

/**
 * Folder Selection Dialog for Destination
 */
void Log2Log::setDstPath() {
    QString initPath = ui->dstPathEdit->text();
    if (initPath.isEmpty())
        initPath = QDir::homePath();
    dstPath = QFileDialog::getExistingDirectory(this, tr("Destination path"), initPath);
    if(!dstPath.isNull())
    {
        ui->dstPathEdit->setText(dstPath);
        dontDefaultDstPath = true;
    }
}

/**
 * Toggle Conversion UI
 */
void Log2Log::toggleConversion()
{
    if (convertMode == false)
        startConversion();
    else
        stopConversion();
}

/**
 * Start Conversion Process
 */
void Log2Log::startConversion()
{
    // Set Mode
    convertMode = true;

    // Hide Main UI
    Helper::convertMode(ui);

    // Show Progress UI
    progress.reset();
    progress.show();
    proginfo.setText("Starting...");
    proginfo.show();

    /* ### GO!!! ### */
    cvHandler = new Conversion(this);
    connect(cvHandler, SIGNAL(updateProgress(int, QString)), this, SLOT(setProgress(int, QString)), Qt::QueuedConnection);
    connect(cvHandler, SIGNAL(updateGui(QString, StdConverter*)), this, SLOT(doGui(QString, StdConverter*)), Qt::QueuedConnection);
    connect(cvHandler, SIGNAL(conversionError(QString)), this, SLOT(handleConversionError(QString)), Qt::QueuedConnection);
    connect(cvHandler, SIGNAL(finished()), this, SLOT(stopConversion()), Qt::QueuedConnection);

    cvHandler->start();
}

/**
 * Stop Conversion Process
 */
void Log2Log::stopConversion()
{
    // Set Mode
    convertMode = false;
    
    // Show Main UI
    Helper::mainMode(ui);
    
    // Hide Progress UI
    if (proginfo.text() != "Conversion complete!")
    {
        progress.hide();
        if (error)
            error = false;
        else
            proginfo.hide();
    }
    
    /* STOP */
    cvHandler->terminate_all();
}

/**
 * Set Progress UI
 */
void Log2Log::setProgress(int meter, QString description)
{
    progress.setValue(meter);
    proginfo.setText(description);
}

/**
 * Handle Conversion Error
 */
void Log2Log::handleConversionError(QString description)
{
    // Abort conversion immediately
    this->stopConversion();

    // Display the error
    proginfo.show();
    proginfo.setText(description);

    // Lock error message
    error = true;
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
    QString html   = "<ul>";
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
                "Messages that are not exactly character-for-character (e.g. voice transcripts) " +
                "will be assumed to be exact in " +
                dstFI->getName("display") +
                "." +
                hEnd;
        cError ++;
    }
    // Precision Index not as precise in destination
    if (srcFI->getTime() > 0 && dstFI->getTime() > 0 && srcFI->getPrecision() < dstFI->getPrecision())
    {
        QString srcPrecisionReadable = Helper::whatPrecision(srcFI->getPrecision());
        QString dstPrecisionReadable = Helper::whatPrecision(dstFI->getPrecision());
        html += hError +
                "Times are not as accurately saved. " +
                srcFI->getName("display") +
                " can store as close as a <b>" +
                srcPrecisionReadable +
                "</b>, but " +
                dstFI->getName("display") +
                " can only store as close as a <b>" +
                dstPrecisionReadable +
                "</b>." +
                hEnd;
        cError ++;
    }
    // Timezone handling
    if (dstFI->getTime() > 0 && dstFI->getTimezone() < srcFI->getTimezone())
    {
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
                Helper::whatTrinary(srcFI->getTimezone()) +
                "</b>, but in " +
                dstFI->getName("display") +
                ", timezones are <b>" +
                Helper::whatTrinary(dstFI->getTimezone()) +
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
 * Check for Updates Dialog Box
 */
void Log2Log::menuUpdates()
{
    Update *w = new Update(this);
    w->show();
}

/**
 * About Log2Log Dialog Box
 */
void Log2Log::menuAbout()
{
    About *w = new About(this);
    w->show();
}

/**
 * Get Ui::Log2Log Pointer
 */
Ui::Log2Log* Log2Log::getUi()
{
    return ui;
}

/**
 * Flexible GUI Control
 */
QWidget* Log2Log::doGui(QString item, StdConverter *callback_object)
{
    QWidget *widget;

    if (item == "MeeboConnectView")
        widget = new MeeboConnectView();

    QVariant result = callback_object->guiCallback(QVariant());

    widget->show();
    return widget;
}
