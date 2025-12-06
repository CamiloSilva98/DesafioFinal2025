#include <QApplication>
#include <QSoundEffect>
#include "juego.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QSoundEffect testSound;
    testSound.setSource(QUrl("qrc:/sounds/click_ok.wav"));
    testSound.setVolume(1.0f);
    testSound.play();

    Juego juego;
    juego.setWindowTitle("Operación Valquiria");
    //juego.resize(800, 600);
    juego.show();

    return app.exec();
}
