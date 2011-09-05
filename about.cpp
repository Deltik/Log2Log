#include "about.h"
#include "ui_about.h"
#include "update.h"
#include <QTimer>

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    connect(ui->backButton, SIGNAL(clicked()), this, SLOT(goBack()));
    connect(ui->creditsButton, SIGNAL(clicked()), this, SLOT(showCredits()));
    connect(ui->licenseButton, SIGNAL(clicked()), this, SLOT(showLicense()));

    // Set Version Number
    ui->textBrowser->setHtml(ui->textBrowser->toHtml().replace("VERSION", (QString)VERSION));

    this->setWindowTitle("About Log2Log");

    this->setWindowOpacity(0.0);
    QTimer* timer = new QTimer;
    timer->setInterval(10);
    connect(timer, SIGNAL(timeout()), this, SLOT(fader()));
    timer->start();
}

About::~About()
{
    delete ui;
}

// Go back / Cancel button
void About::goBack()
{
    this->close();
}

// Show Credits
void About::showCredits()
{
    this->ui->textBrowser->setHtml((QString)"<table width=\"100%\">" +
                                   "<tr><td>Designed by:</td><th>Nick Liu (Deltik)</th></tr>" +
                                   "<tr><td>Inspired by:</td><th>UFGrayMatter</th></tr>" +
                                   "<tr><td>Programmer:</td><th>Nick Liu (Deltik)</th></tr>" +
                                   "<tr><td>Programmer:</td><th>Igor Cardoso (igordcard)</th></tr>" +
                                   "</table>" +
                                   "<p><center><img src=\":/images/etc/Deltik.png\" alt=\"Deltik\" /></center></p>");
}

// Show License
void About::showLicense()
{
    this->ui->textBrowser->setHtml((QString)"<p>Log2Log is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.</p>" +
                                   "<p>Log2Log is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.</p>" +
                                   "<p>You should have received a copy of the GNU General Public License along with Log2Log  If not, see &lt;http://www.gnu.org/licenses/&gt;.</p>");
}

// Sleeper
void About::fader()
{
    this->setWindowOpacity(this->windowOpacity() + 0.025);
}
