#include <QApplication>
#include "mainwidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWidget mainWidget;
    mainWidget.show();
    return app.exec();
}
