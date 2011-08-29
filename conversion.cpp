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

Conversion::Conversion(Ui::Log2Log *ui)
{
    // Configure Conversion UI
    progress.setObjectName("progressBar");
    progress.setFixedSize(200, 16);
    progress.setValue(0);
    proginfo.setObjectName("progressInfo");
    proginfo.setStyleSheet("font-size: 12px;");

    // Place Progress UI
    ui->statusBar->addWidget(&progress);
    ui->statusBar->addWidget(&proginfo);

    // Show Progress UI
    progress.reset();
    progress.show();
    proginfo.setText("Starting...");
    proginfo.show();

    // Data Collection
    proginfo.setText("Collecting Data...");
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
    from["path"]     = ui->srcPassEdit->text();
    from["username"] = ui->srcUserEdit->text();
    from["password"] = ui->srcPassEdit->text();
    to  ["path"]     = ui->dstPassEdit->text();
    to  ["username"] = ui->dstUserEdit->text();
    to  ["password"] = ui->dstPassEdit->text();

    // Load "From" converter class
    //  Unfortunately, since C++ doesn't dynamically load classes, the classes
    //  are loaded by hard-code.
    if (from_name == "Omegle")
        $FROM = new Omegle();

    // Load "To" converter class
    //  Unfortunately, since C++ doesn't dynamically load classes, the classes
    //  are loaded by hard-code.
    if (to_name == "Omegle")
        $TO = new Omegle();

    // Go!
    $FROM->from(from);
}

/**
 * Destructor
 */
Conversion::~Conversion()
{
    // Hide Progress UI
    progress.hide();
    proginfo.hide();
}
