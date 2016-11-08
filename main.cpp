#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("tkyashiro");
    QCoreApplication::setApplicationName("LogViewer");

    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
