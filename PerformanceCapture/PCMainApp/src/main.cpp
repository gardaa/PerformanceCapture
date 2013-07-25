#include "PCMainWindow.h"

#include <QApplication>

#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    PCMainWindow w;
    w.showMaximized ();
    
    return a.exec();
}
