#include <QDebug>
#include "juego.h"
#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>
#include "nivel1.h"
#include "nivel2.h"
#include "nivel3.h"


Juego::Juego(QWidget* parent)
    : QMainWindow(parent),
    nivelActual(nullptr),
    puntuacion(0),
    vidas(3),
    estadoJuego(Estado::MENU),
    nivelActualNumero(0)
{
    // Configurar el timer del juego (60 FPS = ~16ms)
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Juego::actualizar);

    // Configurar la ventana
    setFixedSize(1250, 770);
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
    timer->start(16);
}

void Juego::cambiarNivel(int nivel)
{
    if (nivelActual != nullptr)
    {
        delete nivelActual;
        nivelActual = nullptr;
    }

     nivelActualNumero = nivel;

    // TODO: crear niveles según el número
    switch(nivel)
    {
        case 1:
            nivelActual = new Nivel1();
            nivelActual->inicializar();
            break;

        case 2:
            nivelActual = new Nivel2();
            nivelActual->inicializar();
            break;;

        case 3:
            nivelActual = new Nivel3();
            nivelActual->inicializar();
            break;
        default:
            nivelActual = nullptr;
            break;
    }
}

void Juego::cargarSiguienteNivel()
{
    int siguienteNivel = nivelActualNumero + 1;

    if (siguienteNivel > 3)
    {
        // Ya completó todos los niveles - Victoria final
        qDebug() << " ¡JUEGO COMPLETADO! Todos los niveles superados";
        estadoJuego = Estado::VICTORIA;
        timer->stop();
    }
    else
    {
        // Cargar siguiente nivel
        qDebug() << "️ Cargando nivel" << siguienteNivel << "...";
        cambiarNivel(siguienteNivel);
        estadoJuego = Estado::JUGANDO;
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

        // ============================================
        // VERIFICAR VICTORIA
        // ============================================
        if (nivelActual->verificarCondicionVictoria())
        {
            qDebug() << "¡NIVEL COMPLETADO!";
            estadoJuego = Estado::VICTORIA;
            timer->stop();

            // Aquí puedes agregar puntuación bonus
            actualizarPuntuacion(1000);
        }

        // ============================================
        // VERIFICAR DERROTA
        // ============================================
        if (nivelActual->verificarCondicionDerrota())
        {
            qDebug() << "MISIÓN FALLIDA";
            
            // Diferenciar por nivel
            int numeroNivel = nivelActual->getNumeroNivel();
            
            if (numeroNivel == 1) {
                // Nivel 1: sistema de vidas (3 detecciones permitidas)
                // Ya se maneja internamente en Nivel1
                terminar();  // Game Over
                
            } else if (numeroNivel == 2) {
                // Nivel 2: derrota directa (sin vidas)
                terminar();  // Game Over inmediato
                
            } else if (numeroNivel == 3) {
                // Nivel 3: depende de la fase
                // Fase terrestre: 3 choques permitidos
                // Fase aérea: impacto de misil = game over
                terminar();
            }
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
    // N -> siguiente nivel (solo en VICTORIA)
    case Qt::Key_N:
        if (estadoJuego == Estado::VICTORIA) {
            if (nivelActual && nivelActual->getNumeroNivel() == 1) {
                // Nivel 1 -> Nivel 2
                cambiarNivel(2);
                estadoJuego = Estado::JUGANDO;
                timer->start(16);
            } else if (nivelActual && nivelActual->getNumeroNivel() == 2) {
                // Nivel 2 -> Nivel 3
                cambiarNivel(3);
                estadoJuego = Estado::JUGANDO;
                timer->start(16);
            } else if (nivelActual && nivelActual->getNumeroNivel() == 3) {
                // Nivel 3 -> volver al menú (por ahora fin de juego)
                estadoJuego = Estado::MENU;
                timer->stop();
                delete nivelActual;
                nivelActual = nullptr;
            }
            update();
        }
        break;
    // R -> reiniciar nivel actual (solo en DERROTA)
    case Qt::Key_R:
        if (estadoJuego == Estado::DERROTA) {
            if (nivelActual) {
                int n = nivelActual->getNumeroNivel();
                cambiarNivel(n);          // recarga el mismo nivel
                estadoJuego = Estado::JUGANDO;
                timer->start(16);
                update();
            }
        }
        break;

    // M -> volver al menú desde VICTORIA o DERROTA=
    case Qt::Key_M:
        if (estadoJuego == Estado::VICTORIA || estadoJuego == Estado::DERROTA) {
            estadoJuego = Estado::MENU;
            timer->stop();
            if (nivelActual) {
                delete nivelActual;
                nivelActual = nullptr;
            }
            update();
        }
        break;

    // L -> libro en Nivel 2 (solo jugando)
    case Qt::Key_L:
        if (nivelActual && estadoJuego == Estado::JUGANDO)
        {
            Nivel2* nivel2 = dynamic_cast<Nivel2*>(nivelActual);
            if (nivel2) {
                nivel2->alternarLibro();
            }
        }
        break;
    default:
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
    // Fondo oscuro semi-transparente
    painter.fillRect(rect(), QColor(0, 0, 0, 200));

    painter.setFont(QFont("Arial", 48, QFont::Bold));
    painter.setPen(Qt::green);
    painter.drawText(rect().adjusted(0, -320, 0, 0),
                     Qt::AlignCenter,
                     "¡MISIÓN COMPLETADA!");

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 20));

    QString detalles;

    if (nivelActual && nivelActual->getNumeroNivel() == 1) {
        detalles = QString(
                       "Nivel 1: La Guarida del Lobo\n\n"
                       "Puntuación: %1\n"
                       "Vidas restantes: %2\n\n"
                       "Has conseguido los planos y escapado con vida.\n"
                       "El siguiente objetivo te espera en el Nivel 2."
                       ).arg(puntuacion).arg(vidas);
    }
    else if (nivelActual && nivelActual->getNumeroNivel() == 2) {
        detalles = QString(
                       "Nivel 2: Ensamblaje del Artefacto\n\n"
                       "Puntuación: %1\n\n"
                       "Has ensamblado el dispositivo a la perfección.\n"
                       "El camino se abre hacia el Nivel 3."
                       ).arg(puntuacion);
    }
    else {
        detalles = QString(
                       "Puntuación: %1\n\n"
                       "Has completado la misión actual.\n"
                       "Prepárate para el siguiente desafío."
                       ).arg(puntuacion);
    }

    painter.drawText(rect(), Qt::AlignCenter, detalles);

    // Instrucciones al jugador
    painter.setFont(QFont("Arial", 14));
    painter.drawText(rect().adjusted(0, 260, 0, 0),
                     Qt::AlignCenter,
                     "Pulsa N para continuar al siguiente nivel\n"
                     "Pulsa M para volver al menú principal");
}

void Juego::dibujarDerrota(QPainter& painter)
{
    // Fondo oscuro semi-transparente
    painter.fillRect(rect(), QColor(0, 0, 0, 200));

    painter.setPen(Qt::red);
    painter.setFont(QFont("Arial", 48, QFont::Bold));
    painter.drawText(rect().adjusted(0, -100, 0, 0),
                     Qt::AlignCenter,
                     "MISIÓN FALLIDA");

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 18));
    painter.drawText(rect().adjusted(0, 20, 0, 0),
                     Qt::AlignCenter,
                     "Has perdido esta oportunidad.\n"
                     "Puede haber sido por el tiempo o por demasiados errores.");

    painter.setFont(QFont("Arial", 16));
    painter.drawText(rect().adjusted(0, 120, 0, 0),
                     Qt::AlignCenter,
                     "Presiona R para reiniciar este nivel\n"
                     "Pulsa M para volver al menú principal");
}
