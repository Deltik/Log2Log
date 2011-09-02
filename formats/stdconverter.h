#ifndef STDCONVERTER_H
#define STDCONVERTER_H

#include "formats/stdformat.h"
#include <QtCore>

class StdConverter : public QThread
{
    Q_OBJECT

public:
    /* FUNCTIONS */
    StdConverter();
    ~StdConverter();
    virtual void load(QVariant $log_raw);
    virtual QVariant generate(StdFormat* $log);
    virtual void unset();
    void deleteLog() { this->unset(); }
    void remove() { this->unset(); }
    void destroy() { this->unset(); }
    virtual void setMode(QString input);
    virtual void setInput(QVariant input);
    virtual void setInput(StdFormat* input);
    virtual QVariant getData(QVariant);
    virtual StdFormat* getData(StdFormat*);
    virtual void run();
    /* VARIABLES */
    StdFormat* final;
    QString mode;
    QVariant data;

public slots:
    /* FUNCTIONS */
    virtual StdFormat* from(QHash<QString, QVariant> data);
    virtual void to(StdFormat* $log);

signals:
    void updateProgress(int meter, QString description);
    void finished();
};

#endif // STDCONVERTER_H
