/** @author igordcard */

#ifndef CONVERSION_H
#define CONVERSION_H

#include <QDialog>

namespace Ui {
    class Conversion;
}

class Conversion : public QDialog
{
    Q_OBJECT

public:
    explicit Conversion(QWidget *parent = 0, QIcon *srcIcon = 0, QIcon *dstIcon = 0);
    ~Conversion();

private slots:
    void goBack();
    void progressMock();

private:
    Ui::Conversion *ui;
};

#endif // CONVERSION_H
