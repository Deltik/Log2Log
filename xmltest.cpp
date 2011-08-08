#include "xmltest.h"
#include "ui_xmltest.h"
#include <QDialog>
#include <QXmlStreamReader>
#include <fstream>
#include <iostream>
#include <string>

QXmlStreamReader *xml; // Test
QWidget *mama;

using namespace std;

XmlTest::XmlTest(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::XmlTest)
{
    ui->setupUi(this);
    mama = parent;
    this->setWindowTitle("XML Test - Log2Log");
    xml = new QXmlStreamReader;

    /***********\
    | EXECUTION |
    \***********/
    ifstream fileHandle;
    string data;
    fileHandle.open("/resources/formats.xml");
    if (fileHandle.is_open())
    {
        while (!fileHandle.eof())
        {
            getline(fileHandle, data);
            cout << data;
        }
    }
    else
    {
        cout << "Hmm...";
    }
    ui->textBrowser->setHtml("Everybody is looking for something...");
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
