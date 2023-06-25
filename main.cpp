#include "mainwindow.h"
#include "QMessageBox"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Set the locale to English
    QLocale::setDefault(QLocale::English);
    MainWindow w;
    //w.showFullScreen();
    w.show();

    return a.exec();
}
