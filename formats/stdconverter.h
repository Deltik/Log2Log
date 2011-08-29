#ifndef STDCONVERTER_H
#define STDCONVERTER_H

#include "formats/stdformat.h"
#include <QVariant>

class StdConverter
{
public:
    /* FUNCTIONS */
    StdConverter();
    ~StdConverter();
    QVariant load();
    QVariant generate();
    void unset();
    void deleteLog() { this->unset(); }
    void remove() { this->unset(); }
    void destroy() { this->unset(); }
    StdFormat from(QHash<QString, QVariant> data);
    void to();
    /* VARIABLES */
    StdFormat *final;
};

#endif // STDCONVERTER_H
