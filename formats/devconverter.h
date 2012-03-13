#ifndef DEVCONVERTER_H
#define DEVCONVERTER_H

#include "formats/stdformat.h"
#include "formats/stdconverter.h"
#include "api.h"
#include <QtCore>

class DevConverter : public StdConverter
{
    Q_OBJECT

public:
    DevConverter();
    virtual void load(QVariant $log_raw);
    virtual QVariant generate(StdFormat* $log);

public slots:
    virtual StdFormat* from(QHash<QString, QVariant> data);
    virtual void to(StdFormat* $log);
    virtual QVariant guiCallback(QVariant data);
    void progressBarTest(qint64 a, qint64 b);

private:
    int total;
};

#endif // DEVCONVERTER_H
