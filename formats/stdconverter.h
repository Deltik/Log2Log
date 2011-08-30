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
    virtual void load();
    virtual void generate();
    virtual void unset();
    void deleteLog() { this->unset(); }
    void remove() { this->unset(); }
    void destroy() { this->unset(); }
    virtual StdFormat* from(QHash<QString, QVariant> data);
    virtual void to();
    /* VARIABLES */
    StdFormat* final;
};

#endif // STDCONVERTER_H
