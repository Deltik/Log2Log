#include "conversion.h"

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
    from["path"]     = ui->srcPassEdit->text();
    from["username"] = ui->srcUserEdit->text();
    from["password"] = ui->srcPassEdit->text();
    to  ["path"]     = ui->dstPassEdit->text();
    to  ["username"] = ui->dstUserEdit->text();
    to  ["password"] = ui->dstPassEdit->text();
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
