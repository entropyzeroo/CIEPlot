#include "ciedemo.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CIEDemo w;
    w.show();
    return a.exec();
}
