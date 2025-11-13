#include <QApplication>
#include "juego.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Juego juego;
    juego.setWindowTitle("Operación Valquiria");
    juego.resize(800, 600);
    juego.show();

    return app.exec();
}
