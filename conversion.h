#ifndef CONVERSION_H
#define CONVERSION_H

#include "ui_log2log.h"
#include "formats/stdformat.h"
#include "formats/stdconverter.h"
#include <QProgressBar>
#include <QLabel>
#include <QHash>

class Conversion
{
public:
    Conversion();
    Conversion(Ui::Log2Log *ui);
    ~Conversion();

private:
    // Variables: Conversion UI
    QProgressBar progress;
    QLabel proginfo;
    // Variables: Data Collection
    QString from_name;
    QString to_name;
    QHash<QString, QVariant> from;
    QHash<QString, QVariant> to;
    // Variables: Format Converter Objects
    StdConverter* $FROM;
    StdConverter* $TO;
    // Variables: Chat Log
    StdFormat final;
};

#endif // CONVERSION_H
