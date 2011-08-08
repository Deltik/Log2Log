#include "xmltest.h"
#include "ui_xmltest.h"
#include <QDialog>
#include <QXmlStreamReader>

QXmlStreamReader *xml; // Test
QWidget *mama;

XmlTest::XmlTest(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::XmlTest)
{
    ui->setupUi(this);
    mama = parent;
    xml = new QXmlStreamReader;
}

XmlTest::~XmlTest()
{
    delete ui;
}

/**
 * Cancel Button
 */
void XmlTest::on_xButton_clicked()
{
    mama->show();
    this->close();
}
