#include "xmltest.h"
#include "ui_xmltest.h"
#include <QDialog>
#include <QXmlStreamReader>
#include <QXmlSimpleReader>
#include <QFile>
#include <QString>
#include <QtDebug>
#include <fstream>
#include <iostream>
#include <string>

QXmlStreamReader *xml; // Test
QWidget *mama;
QFile *xmlsrc;

using namespace std;

XmlTest::XmlTest(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::XmlTest)
{
    ui->setupUi(this);
    mama = parent;
    this->setWindowTitle("XML Test - Log2Log");
    QFile xmlsrc(":/resources/formats.xml");

    /***********\
    | EXECUTION |
    \***********/
    QString data;
    if (!xmlsrc.open(QIODevice::ReadOnly | QIODevice::Text))
        cout << "FAIL";
    while (!xmlsrc.atEnd())
    {
        QByteArray line = xmlsrc.readLine();
        data += line.data();
    }
    QXmlStreamReader xml(data);
    ui->textBrowser->setPlainText("Everybody is looking for something...");
    ui->textBrowser->setPlainText(data);
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
