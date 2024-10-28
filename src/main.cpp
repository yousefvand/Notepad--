#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qRegisterMetaType<QStringList>("QStringList");

    MainWindow w;
    w.show();

    return app.exec();
}
