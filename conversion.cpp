#include "conversion.h"
#include "formatinfo.h"

/* INCLUDE FORMAT CONVERTER CLASSES */
#include "formats/stdformat.h"
#include "formats/omegle.h"


/**
 * Constructor
 */
Conversion::Conversion()
{
}

Conversion::Conversion(Ui::Log2Log *parentUi)
{
    ui = parentUi;
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

    // Load "From" converter class
    //  Unfortunately, since C++ doesn't dynamically load classes, the classes
    //  are loaded by hard-code.
    emit updateProgress(0, "Loading Source Converter...");
    $FROM = new StdConverter();
    if (from_name == "Omegle")
        $FROM = new Omegle();

    // Load "To" converter class
    //  Unfortunately, since C++ doesn't dynamically load classes, the classes
    //  are loaded by hard-code.
    emit updateProgress(0, "Loading Destination Converter...");
    $TO = new StdConverter();
    if (to_name == "Omegle")
        $TO = new Omegle();

    // Generously (and prettily) run files_get_contents, if applicable
    if (!from["path"].toString().isEmpty())
    {
        from["files"] = files_get_contents(from["path"].toString());
    }

    // Go!
    $FROM->from(from);

    emit done();
}

/**
 * DEBUG: Do Dummy Work
 */
void Conversion::doDummyWork()
{
    sleep(3);
    qDebug()<<"Dummy work done.";
    emit finished();
}

/**
 * Destructor
 */
Conversion::~Conversion()
{
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

    QDirIterator directory_counter(directory_path, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    int t = 0;
    emit updateProgress(0, "Counting files...");
    while (directory_counter.hasNext()) { directory_counter.next(); t ++; }

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
            fileContents += line;
        }
        list[directory_walker.filePath()] = fileContents;
        emit updateProgress((10 * n / t), "Loaded " + QVariant(n).toString() + "/" + QVariant(t).toString() + " files...");
    }

    // Return the list that this method made. :D
    return list;
}
