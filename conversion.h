#ifndef CONVERSION_H
#define CONVERSION_H

#include "ui_log2log.h"
#include <QProgressBar>
#include <QLabel>

class Conversion
{
public:
    Conversion();
    Conversion(Ui::Log2Log *ui);
    ~Conversion();

private:
    // Variables Conversion UI
    QProgressBar progress;
    QLabel proginfo;
};

#endif // CONVERSION_H
