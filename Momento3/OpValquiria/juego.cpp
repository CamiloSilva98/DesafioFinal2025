#include <QDebug>
#include "juego.h"
#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>
#include "nivel1.h"
#include "nivel2.h"

Juego::Juego(QWidget* parent)
    : QMainWindow(parent),
    nivelActual(nullptr),
    puntuacion(0),
    vidas(3),
    estadoJuego(Estado::MENU)
{
    // Configurar el timer del juego (60 FPS = ~16ms)
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Juego::actualizar);

    // Configurar la ventana
    setFixedSize(1550, 770);
    setFocusPolicy(Qt::StrongFocus);
}

Juego::~Juego()
{
    if (nivelActual != nullptr)
    {
        delete nivelActual;
    }
}

void Juego::iniciar()
{
    estadoJuego = Estado::JUGANDO;
    puntuacion = 0;
    vidas = 3;


    //cambiarNivel(1); // Iniciar el nivel 1
    //timer->start(16); // ~60 FPS // Iniciar el loop del juego

    // Iniciar directamente el nivel 2
    cambiarNivel(2);
    timer->start(16);
}

void Juego::cambiarNivel(int nivel)
{
    // Por ahora solo un placeholder
    // Después crearán los niveles específicos
    if (nivelActual != nullptr)
    {
        delete nivelActual;
        nivelActual = nullptr;
    }

    // TODO: crear niveles según el número
    switch(nivel)
    {
        //case 1:
        //nivelActual = new Nivel1();
        //nivelActual->inicializar();
        //break;

        case 2:
            nivelActual = new Nivel2();
            break;
    //     case 2: nivelActual = new Nivel2Ensamblaje(); break;
    //     case 3: nivelActual = new Nivel3Escape(); break;
    }

    if (nivelActual) {
        nivelActual->inicializar();
     }

}

void Juego::actualizarPuntuacion(int puntos)
{
    puntuacion += puntos;
    update(); // Redibuja la pantalla
}

void Juego::pausar() {
    if (estadoJuego == Estado::JUGANDO)
    {
        estadoJuego = Estado::PAUSADO;
        timer->stop();
    } else if (estadoJuego == Estado::PAUSADO)
    {
        estadoJuego = Estado::JUGANDO;
        timer->start(16);
    }
}

void Juego::terminar()
{
    timer->stop();
    estadoJuego = Estado::DERROTA;
    update();
}

void Juego::actualizar()
{
    if (estadoJuego == Estado::JUGANDO && nivelActual != nullptr)
    {
        nivelActual->actualizar(0.016f); // delta time

        if (nivelActual->verificarCondicionVictoria()) {
            estadoJuego = Estado::VICTORIA;
            timer->stop();
        } else if (nivelActual->verificarCondicionDerrota()) {
            // Diferenciar por nivel
            if (nivelActual->getNumeroNivel() == 1) {
                // Nivel 1: usar sistema de vidas
                vidas--;
                if (vidas > 0) {
                    cambiarNivel(1);  // reiniciar nivel 1
                } else {
                    terminar();  // Game Over
                }
            } else if (nivelActual->getNumeroNivel() == 2) {
                // Nivel 2: derrota directa (sin vidas)
                terminar();  // Game Over inmediato
            }
        }
    }

    update(); // Redibuja
}

void Juego::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_Escape:
        pausar();
        break;
    case Qt::Key_Space:
        if (estadoJuego == Estado::MENU)
        {
            iniciar();
        }
        break;
    default:
        // ⭐ PASAR TODAS LAS TECLAS AL NIVEL
        if (nivelActual && estadoJuego == Estado::JUGANDO)
        {
            Nivel1* nivel1 = dynamic_cast<Nivel1*>(nivelActual);
            if (nivel1) {
                nivel1->manejarTecla(event, true);  // true = presionada
            }
        }
        break;
    }
}

void Juego::keyReleaseEvent(QKeyEvent* event)
{
    if (nivelActual && estadoJuego == Estado::JUGANDO)
    {
        Nivel1* nivel1 = dynamic_cast<Nivel1*>(nivelActual);
        if (nivel1) {
            nivel1->manejarTecla(event, false);  // false = soltada
        }
    }
}

void Juego::mousePressEvent(QMouseEvent* event)
{
    if (!nivelActual || estadoJuego != Estado::JUGANDO) return;

    Nivel2* n2 = dynamic_cast<Nivel2*>(nivelActual);
    if (n2) {
        n2->manejarMousePress(event->pos());
    }
}

void Juego::mouseMoveEvent(QMouseEvent* event)
{
    if (!nivelActual || estadoJuego != Estado::JUGANDO) return;

    Nivel2* n2 = dynamic_cast<Nivel2*>(nivelActual);
    if (n2) {
        n2->manejarMouseMove(event->pos());
    }
}

void Juego::mouseReleaseEvent(QMouseEvent* event)
{
    if (!nivelActual || estadoJuego != Estado::JUGANDO) return;

    Nivel2* n2 = dynamic_cast<Nivel2*>(nivelActual);
    if (n2) {
        n2->manejarMouseRelease(event->pos());
    }
}



void Juego::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    // Fondo negro
    painter.fillRect(0, 0, width(), height(), Qt::black);

    switch (estadoJuego)
    {
    case Estado::MENU:
        dibujarMenu(painter);
        break;
    case Estado::JUGANDO:
        if (nivelActual != nullptr)
        {
            nivelActual->renderizar(&painter);
        }

        // Solo mostrar HUD de vidas/puntuación en el Nivel 1
        if (nivelActual && nivelActual->getNumeroNivel() == 1) {
            dibujarHUD(painter);
        }
        break;
    case Estado::PAUSADO:
        if (nivelActual != nullptr)
        {
            nivelActual->renderizar(&painter);
        }
        dibujarPausa(painter);
        break;
    case Estado::VICTORIA:
        dibujarVictoria(painter);
        break;
    case Estado::DERROTA:
        dibujarDerrota(painter);
        break;
    }
}

void Juego::dibujarMenu(QPainter& painter)
{
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 32, QFont::Bold));
    painter.drawText(rect(), Qt::AlignCenter, "OPERACIÓN VALQUIRIA");

    painter.setFont(QFont("Arial", 16));
    painter.drawText(rect().adjusted(0, 100, 0, 0), Qt::AlignCenter,
                     "Presiona ESPACIO para comenzar");
}

void Juego::dibujarHUD(QPainter& painter)
{
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 11));

    // Puntuación
    painter.drawText(10, 32, QString("Puntuación: %1").arg(puntuacion));

    // Vidas
    painter.drawText(10, 60, QString("Vidas: %1").arg(vidas));
}

void Juego::dibujarPausa(QPainter& painter)
{
    // Semi-transparencia
    painter.fillRect(rect(), QColor(0, 0, 0, 180));

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 24, QFont::Bold));
    painter.drawText(rect(), Qt::AlignCenter, "PAUSA\n\nESC para continuar");
}

void Juego::dibujarVictoria(QPainter& painter)
{
    painter.setPen(Qt::green);
    painter.setFont(QFont("Arial", 32, QFont::Bold));
    painter.drawText(rect(), Qt::AlignCenter,
                     QString("¡VICTORIA!\n\nPuntuación: %1").arg(puntuacion));
}

void Juego::dibujarDerrota(QPainter& painter)
{
    painter.setPen(Qt::red);
    painter.setFont(QFont("Arial", 32, QFont::Bold));
    painter.drawText(rect(), Qt::AlignCenter, "MISIÓN FALLIDA");
}
