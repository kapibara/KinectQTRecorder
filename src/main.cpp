#include "qmainwindow.h"
#include <QApplication>

int main(int argc, char **argv){

     QApplication a(argc, argv);
     ResourceStorage s(argv[0]);

     QMainWindow w(s);

     w.show();

     return a.exec();
}

