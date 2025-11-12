#include "TransportRoute.h"
#include <QtWidgets/QApplication>
#include <QDebug>
#include <QtGlobal>
#include <QMessageBox>
#include <QString>
#include <QLatin1String>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    TransportRoute window;
    window.show();


    QString msg = QString("Aplicación iniciada.\nQt versión en tiempo de ejecución: %1")
                      .arg(QLatin1String(qVersion()));
    QMessageBox::information(&window, "Inicio", msg);

    return app.exec();
}


