#include "PcMainWindow.h"

#include <QApplication>

#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    PcMainWindow w;
    w.showMaximized ();
    
    return a.exec();
}
