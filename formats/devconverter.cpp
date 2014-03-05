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
    emit updateGui("MeeboConnectView", this);

    Api *test = new Api();
    connect(test, SIGNAL(progress(qint64,qint64)), this, SLOT(progressBarTest(qint64,qint64)));
    test->setURL("http://dl.google.com/linux/direct/google-talkplugin_current_amd64.deb");

    test->start();
    //QWaitCondition waiter;
    //waiter.wait(test->lck);
    test->wait();

    for (int i = 5; i > 0; i --)
    {
        for (int j = 0; j < 10; j ++)
        {
            updateProgress((5-i)*10+j, "Stalling for "+QVariant(i).toString()+" seconds...");
            msleep(100);
        }
    }
    emit finished();
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
    emit finished();
}

QVariant DevConverter::guiCallback(QVariant data)
{
    qDebug() << "GUI Callback on DevConverter!";
}

void DevConverter::progressBarTest(qint64 a, qint64 b)
{
    updateProgress((a*100)/(b*100), QVariant(a).toString()+"/"+QVariant(b).toString());
}
