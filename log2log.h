/**
 * @author Deltik
 * @author igordcard
 */

#ifndef LOG2LOG_H
#define LOG2LOG_H

#include <QMainWindow>
#include <QLayoutItem>

#define VERSION "1.0.0"

namespace Ui {
    class Log2Log;
}

class Log2Log : public QMainWindow
{
    Q_OBJECT

public:
    explicit Log2Log(QWidget *parent = 0);
    ~Log2Log();

protected:
    void updateVisibleFields(int arg1, int arg2);

private slots:
    void quit();
    void menuAbout();
    void updateFields();
    void setSrcPath();
    void setDstPath();
    void startConversion();
    void whatsWrong(int srcIndex = NULL, int dstIndex = NULL);

private:
    Ui::Log2Log *ui;
    QString srcPath;
    QString dstPath;
};

#endif // WIZARD_H
