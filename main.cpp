#include <QtGui/QApplication>
#include "log2log.h"

#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Log2Log w;

    w.show();

    return a.exec();
}
