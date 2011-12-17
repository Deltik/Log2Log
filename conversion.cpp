/**
 * Log2Log Chat Log Converter
 *  Execution
 *   Format Converter Handler
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

#include "conversion.h"
#include "formatinfo.h"
#include <utime.h>

/* INCLUDE FORMAT CONVERTER CLASSES */
#include "formats/stdformat.h"
#include "formats/omegle.h"
#include "formats/pidgin.h"
#include "formats/meebo.h"
#include "formats/meeboconnect.h"
#include "formats/stdjson.h"
#include "formats/trillian.h"
#include "formats/wlm.h"
#include "formats/aim.h"
#include "formats/skype.h"


/**
 * Constructor for ConversionConsole
 */
Conversion::Conversion(QHash<QString, QVariant> import)
{
    // Load each indexs' profiles
    FormatInfo* srcFI = new FormatInfo(import["from"].toString());
    srcFI->pointerDig();
    from_name = srcFI->getName("class");
    FormatInfo* dstFI = new FormatInfo(import["to"].toString());
    dstFI->pointerDig();
    to_name = dstFI->getName("class");
    //  Load form information
    from["path"]     = import["source"]              .toString();
    from["username"] = import["source-username"]     .toString();
    from["password"] = import["source-password"]     .toString();
    to  ["path"]     = import["destination"]         .toString();
    to  ["username"] = import["destination-username"].toString();
    to  ["password"] = import["destination-password"].toString();

    moveToThread(this);
}

/**
 * Constructor for Ui::Log2Log
 */
Conversion::Conversion(Ui::Log2Log *parentUi)
{
    ui = parentUi;
    // Data Collection
    emit updateProgress(0, "Reading Form Data...");
    int srcIndex = ui->srcProtoBox->currentIndex();
    int dstIndex = ui->dstProtoBox->currentIndex();
    //  Load each indexs' profiles
    FormatInfo* srcFI = new FormatInfo(ui->srcProtoBox->itemData(srcIndex).toString());
    srcFI->pointerDig();
    from_name = srcFI->getName("class");
    FormatInfo* dstFI = new FormatInfo(ui->dstProtoBox->itemData(dstIndex).toString());
    dstFI->pointerDig();
    to_name = dstFI->getName("class");
    //  Load form information
    from["path"]     = ui->srcPathEdit->text();
    from["username"] = ui->srcUserEdit->text();
    from["password"] = ui->srcPassEdit->text();
    to  ["path"]     = ui->dstPathEdit->text();
    to  ["username"] = ui->dstUserEdit->text();
    to  ["password"] = ui->dstPassEdit->text();

    moveToThread(this);
}

/**
 * Thread Runner
 */
void Conversion::run()
{
    QTimer::singleShot(0, this, SLOT(collectData()));

    exec();
}

/**
 * Data Collection
 */
void Conversion::collectData()
{
    // Load "From" converter class
    //  Unfortunately, since C++ doesn't dynamically load classes, the classes
    //  are loaded by hard-code.
    emit updateProgress(0, "Loading Source Converter...");
    $FROM = new StdConverter();
    if (from_name == "Omegle")
        $FROM = new Omegle();
    if (from_name == "Pidgin")
        $FROM = new Pidgin();
    if (from_name == "Meebo")
        $FROM = new Meebo();
    if (from_name == "MeeboConnect")
        $FROM = new MeeboConnect();
    if (from_name == "StdJson")
        $FROM = new StdJson();
    if (from_name == "Trillian")
        $FROM = new Trillian();
    if (from_name == "Wlm")
        $FROM = new Wlm();
    if (from_name == "Aim")
        $FROM = new Aim();
    if (from_name == "Skype")
        $FROM = new Skype();

    // Load "To" converter class
    //  Unfortunately, since C++ doesn't dynamically load classes, the classes
    //  are loaded by hard-code.
    emit updateProgress(0, "Loading Destination Converter...");
    $TO = new StdConverter();
    if (to_name == "Omegle")
        $TO = new Omegle();
    if (to_name == "Pidgin")
        $TO = new Pidgin();
    if (to_name == "Meebo")
        $TO = new Meebo();
    if (to_name == "MeeboConnect")
        $TO = new MeeboConnect();
    if (to_name == "StdJson")
        $TO = new StdJson();
    if (to_name == "Trillian")
        $TO = new Trillian();
    if (to_name == "Wlm")
        $TO = new Wlm();
    if (to_name == "Aim")
        $TO = new Aim();
    if (to_name == "Skype")
        $TO = new Skype();

    // Generously (and prettily) run files_get_contents, if applicable
    if (!from["path"].toString().isEmpty())
    {
        from["files"] = files_get_contents(from["path"].toString());
    }

    convertFrom();
}

/**
 * Convert "From"
 */
void Conversion::convertFrom()
{
    // Go!
    connect($FROM, SIGNAL(updateProgress(int, QString)), this, SLOT(setProgressProto(int, QString)));
    connect($FROM, SIGNAL(error(QString)), this, SLOT(error(QString)));
    $FROM->setMode("from");
    $FROM->setInput(QVariant(from));
    $FROM->start();

    connect($FROM, SIGNAL(finished()), this, SLOT(convertTo()), Qt::QueuedConnection);

    emit done();
}

/**
 * Convert "To"
 */
void Conversion::convertTo()
{
    // Destroy $FROM thread
    $FROM->disconnect();
    $FROM->terminate();

    // Go!
    connect($TO, SIGNAL(updateProgress(int, QString)), this, SLOT(setProgressProto(int, QString)));
    connect($FROM, SIGNAL(error(QString)), this, SLOT(error(QString)));

    final = $FROM->getData(new StdFormat);
    $TO->setMode("to");
    $TO->setInput(final);
    $TO->start();

    connect($TO, SIGNAL(finished()), this, SLOT(save()));

    emit done();
}

/**
 * Save Converted Data
 */
void Conversion::save()
{
    updateProgress(90, "Saving...");

    QVariant log_raw = $TO->getData(QVariant());
    QMap<QString, QVariant> log = log_raw.toMap();

    QMap<QString, QVariant>::const_iterator i = log.constBegin();
    int c = 0;

    while (i != log.constEnd())
    {
        // Extract
        QString key = QDir::fromNativeSeparators(to["path"].toString()) + "/" + i.key();
        QVariant value = i.value();
        QHash<QString, QVariant> info = value.toHash();
        // Assume that info["content"] has the file contents
        QVariant content = info["content"];
        // Compatibility: If info["data"] actually has the file contents
        if (content.isNull())
            content = info["data"];
        // Compatibility: If info["text"] actually has the file contents
        if (content.isNull())
            content = info["text"];
        // Compatibility: If just QVariant value is the file contents
        if (content.isNull())
            content = value;

        // Ensure that the directory will be available to write to.
        QStringList key_proc = key.split("/");
        key_proc.pop_back();
        QString dir_path = key_proc.join("/");
        QDir* dir = new QDir();
        dir->mkpath(dir_path);

        // Create file handler
        QFile file(key);
        // Open the file
        file.open(QIODevice::WriteOnly);
        // ### SAVE! ###
        file.write(content.toByteArray());
        // Close the file
        file.close();

        // If the format wants to set the file access and/or modification time
        // (Tested to work on Linux)
        if (!info["modtime"].isNull() || !info["actime"].isNull())
        {
            struct utimbuf qtimebuf;
            QDateTime time;
            qtimebuf.modtime = time.currentDateTime().toTime_t();
            qtimebuf.actime = time.currentDateTime().toTime_t();
            if (!info["modtime"].isNull())
                qtimebuf.modtime = info["modtime"].toUInt();
            if (!info["actime"].isNull())
                qtimebuf.actime = info["modtime"].toUInt();
            utime(key.toStdString().c_str(), &qtimebuf);
        }

        c++; i++;
        updateProgress((10 * c / log.count()) + 90, "Saved "+QVariant(c).toString()+"/"+QVariant(log.count()).toString()+" files...");
    }

    $TO->disconnect();

    // ### DONE! ###
    updateProgress(100, "Conversion complete!");
    emit finished();
}

/**
 * Destructor
 * @deprecated This is not used.
 */
void Conversion::terminate_all()
{
    $FROM->disconnect();
    $TO->disconnect();
    $FROM->terminate();
    $TO->terminate();
    this->terminate();
}

/**
 * Get Multiple Files' Contents
 * @returns QMap<QString, QVariant> where QString is the filename and QVariant
 *          is the file contents
 */
QMap<QString, QVariant> Conversion::files_get_contents(QString directory_path)
{
    // List of files like: list[FILENAME] = FILE_CONTENTS;
    QMap<QString, QVariant> list;

    // Pre-count the files so the user knows how many to expect
    QDirIterator directory_counter(directory_path, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    int t = 0;
    emit updateProgress(0, "Counting files...");
    while (directory_counter.hasNext()) { directory_counter.next(); t ++; emit updateProgress(0, "Counting files... ("+QVariant(t).toString()+" found so far)"); }

    // QDirIterator goes through files recursively in a directory. :)
    QDirIterator directory_walker(directory_path, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);

    // While directory_walker has another file to go through
    int n = 0;
    while (directory_walker.hasNext())
    {
        // Next!
        directory_walker.next();
        n ++;

        // Load File
        QString fileContents = "";
        QFile file(directory_walker.filePath());
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream in(&file);
        while (!in.atEnd())
        {
            QString line = in.readLine();
            if (!in.atEnd())
                fileContents += line + "\n";
            else
                fileContents += line;
        }
        list[directory_walker.filePath()] = fileContents;
        emit updateProgress((10 * n / t), "Loaded " + QVariant(n).toString() + "/" + QVariant(t).toString() + " files...");
    }

    // Return the list that this method made. :D
    return list;
}

/**
 * Pass Progress UI
 */
void Conversion::setProgressProto(int meter, QString description)
{
    emit updateProgress(meter, description);
}

/**
 * Error Handler
 */
void Conversion::error(QString text)
{
    $FROM->terminate();
    $TO->terminate();

    if (!text.isEmpty())
        emit conversionError("<span style=\"color: red;\"><strong>ERROR:</strong> " + text + "</span>");
    else
        emit conversionError("<span style=\"color: red;\"><strong>ERROR:</strong> Conversion failed</span>");
}
