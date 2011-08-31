#ifndef CONVERSION_H
#define CONVERSION_H

#include "ui_log2log.h"
#include "formats/stdformat.h"
#include "formats/stdconverter.h"
#include <QtCore>

class Conversion : public QThread
{
    Q_OBJECT

public:
    Conversion();
    Conversion(Ui::Log2Log *ui);
    ~Conversion();

protected:
    void run();

public slots:
    void collectData();
    void doDummyWork();

private:
    // Variables: Main UI
    Ui::Log2Log *ui;
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
    // Functions
    QMap<QString, QVariant> files_get_contents(QString directory_path);

signals:
    void done();
    void updateProgress(int meter, QString description);
};

#endif // CONVERSION_H
