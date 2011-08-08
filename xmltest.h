/**
 * @author Deltik
 */

#ifndef XMLTEST_H
#define XMLTEST_H

#include <QDialog>

namespace Ui {
    class XmlTest;
}

class XmlTest : public QDialog
{
    Q_OBJECT

public:
    explicit XmlTest(QWidget *parent);
    ~XmlTest();

protected slots:
    void on_xButton_clicked();

private:
    Ui::XmlTest *ui;
};

#endif // XMLTEST_H
