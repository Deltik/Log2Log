#include "devconverter.h"

/**
 * Constructor
 */
DevConverter::DevConverter()
{
    final = new StdFormat();
    final->setClient("Log2Log-Dev");
}

void DevConverter::load(QVariant $log_raw)
{
    ;
}

QVariant DevConverter::generate(StdFormat *$log)
{
    ;
}

/**
 * Process "From" Request
 */
StdFormat* DevConverter::from(QHash<QString, QVariant> data)
{
    QHash<QString, QVariant> test;
    test["item"] = "MeeboConnectView";
    emit updateGui("MeeboConnectView", this);

    for (int i = 5; i > 0; i --)
    {
        for (int j = 0; j < 10; j ++)
        {
            updateProgress((5-i)*10+j, "Stalling for "+QVariant(i).toString()+" seconds...");
            msleep(100);
        }
    }
    emit QThread::finished();
    return this->final;
}

/**
 * Process "To" Request
 */
void DevConverter::to(StdFormat* $log)
{
    for (int i = 5; i > 0; i --)
    {
        for (int j = 0; j < 10; j ++)
        {
            updateProgress((5-i)*10+j+50, "Stalling for "+QVariant(i).toString()+" seconds...");
            msleep(100);
        }
    }
    emit QThread::finished();
}

QVariant DevConverter::guiCallback(QVariant data)
{
    qDebug() << "GUI Callback on DevConverter!";
}
