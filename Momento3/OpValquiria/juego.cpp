#include <QDebug>
#include "juego.h"
#include <QPainter>
#include <QKeyEvent>
#include "nivel1.h"

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

    // Iniciar el nivel 1 (lo implementarán después)
    cambiarNivel(1);

    // Iniciar el loop del juego
    timer->start(16); // ~60 FPS
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
        case 1:
        nivelActual = new Nivel1();
        nivelActual->inicializar();
        break;
    //     case 2: nivelActual = new Nivel2Ensamblaje(); break;
    //     case 3: nivelActual = new Nivel3Escape(); break;
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
    // Este método se llama 60 veces por segundo

    if (estadoJuego == Estado::JUGANDO && nivelActual != nullptr)
    {
        // Actualizar el nivel actual
        nivelActual->actualizar(0.016f); // delta time

        if (nivelActual->verificarCondicionVictoria())
        {
            qDebug() << "¡NIVEL COMPLETADO!";
            estadoJuego = Estado::VICTORIA;
            timer->stop();

            // Aquí puedes agregar puntuación bonus
            actualizarPuntuacion(1000);
        }

        // ⭐ Verificar condiciones de derrota
        if (nivelActual->verificarCondicionDerrota())
        {
            qDebug() << "MISIÓN FALLIDA";
            terminar();
        }
    }

    update(); // Llama a paintEvent()
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
        dibujarHUD(painter);
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
    // Fondo oscuro semi-transparente
    painter.fillRect(rect(), QColor(0, 0, 0, 200));

    // Título de victoria
    painter.setPen(Qt::green);
    painter.setFont(QFont("Arial", 48, QFont::Bold));
    painter.drawText(rect().adjusted(0, -350, 0, 0), Qt::AlignCenter, "¡MISIÓN COMPLETADA!");

    // Detalles
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 20));

    QString detalles = QString(
                           "Nivel 1: La Guarida del Lobo\n\n"
                           "Puntuación: %1\n"
                           "Vidas restantes: %2\n\n"
                           "¡Has obtenido los planos y escapado con éxito!"
                           ).arg(puntuacion).arg(vidas);

    painter.drawText(rect(), Qt::AlignCenter, detalles);

    // Instrucciones
    painter.setFont(QFont("Arial", 14));
    painter.drawText(rect().adjusted(0, 250, 0, 0), Qt::AlignCenter,
                     "Presiona ESPACIO para continuar al siguiente nivel\n"
                     "Presiona ESC para volver al menú");
}

void Juego::dibujarDerrota(QPainter& painter)
{
    painter.setPen(Qt::red);
    painter.setFont(QFont("Arial", 35, QFont::Bold));
    painter.drawText(rect(), Qt::AlignCenter, "MISIÓN FALLIDA");
}
