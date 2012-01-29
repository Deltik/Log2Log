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
    test["action"] = "make";
    test["qml"] = "import Qt 4.7\n\nRectangle {\n  width: 640\n  height: 480\n  color: \"#7bffff00\"\n\n  Text {\n    text: \"I'm walking in the air\"\n    anchors.centerIn: parent\n  }\n}";
    emit this->updateGui(test);

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
