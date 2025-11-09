#include "TransportRoute.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    TransportRoute window;
    window.show();
    return app.exec();
}


