/** @author igordcard */

#ifndef LOG2LOG_H
#define LOG2LOG_H

#include <QDialog>
#include <QLayoutItem>

namespace Ui {
    class Log2Log;
}

class Log2Log : public QDialog
{
    Q_OBJECT

public:
    explicit Log2Log(QWidget *parent = 0);
    ~Log2Log();

protected:
    void updateVisibleFields(int arg1, int arg2);

private slots:
    void updateSrcFields(int index);
    void updateDstFields(int index);
    void setSrcPath();
    void setDstPath();
    void startConversion();

    /* DEBUG */ void on_testButton_clicked();

private:
    Ui::Log2Log *ui;
    QString srcPath;
    QString dstPath;
};

#endif // WIZARD_H
