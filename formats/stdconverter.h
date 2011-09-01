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
    virtual void generate();
    virtual void unset();
    void deleteLog() { this->unset(); }
    void remove() { this->unset(); }
    void destroy() { this->unset(); }
    virtual void setMode(QString input);
    virtual void setInput(QVariant input);
    virtual void run();
    /* VARIABLES */
    StdFormat* final;
    QString mode;
    QVariant data;

public slots:
    /* FUNCTIONS */
    virtual StdFormat* from(QHash<QString, QVariant> data);
    virtual void to();

signals:
    void updateProgress(int meter, QString description);
};

#endif // STDCONVERTER_H
