#include <QtGui/QApplication>
#include "log2log.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Log2Log w;

    w.show();

    return a.exec();
}
