/**
 * @author Deltik
 * @author igordcard
 */

#ifndef LOG2LOG_H
#define LOG2LOG_H

#include <QMainWindow>
#include <QLayoutItem>
#include <QProgressBar>
#include <QLabel>
#include "conversion.h"

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
    void menuUpdates();
    void menuAbout();
    void updateFields();
    void determineSrcDefault(QString newValue);
    void determineDstDefault(QString newValue);
    void setSrcPath();
    void setDstPath();
    void toggleConversion();
    void startConversion();
    void stopConversion();
    void setProgress(int meter, QString description);
    void whatsWrong(int srcIndex = NULL, int dstIndex = NULL);

private:
    Ui::Log2Log *ui;
    bool dontDefaultSrcPath;
    bool dontDefaultDstPath;
    QString srcPath;
    QString dstPath;
    // Variables: Conversion UI
    bool convertMode;
    QProgressBar progress;
    QLabel proginfo;
    // Variables:: Conversion
    Conversion *cvHandler;
};

#endif // WIZARD_H
