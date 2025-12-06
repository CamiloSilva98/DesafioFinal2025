#include "puerta.h"
#include "jugador.h"
#include <QPainter>
#include <QDebug>

Puerta::Puerta(float x, float y)
    : ObjetoInteractivo(x, y, 64, 64, TipoObjeto::PUERTA, "puerta_salida"),
    abierta(false),
    animando(false),
    frameActual(0),
    tiempoAnimacion(0.0f),
    velocidadAnimacion(0.2f)  // 0.2 segundos por frame
{
    sprites.resize(4);
    cargarSprites();
}

Puerta::~Puerta()
{
}

void Puerta::cargarSprites()
{
    qDebug() << "=== CARGANDO SPRITES DE PUERTA ===";

    // Frame 0: Puerta cerrada
    sprites[0] = QPixmap(":/sprites/sprites/Puerta1.png");
    qDebug() << "Puerta cerrada:" << !sprites[0].isNull();

    // Frame 1: Animación abriendo (medio)
    sprites[1] = QPixmap(":/sprites/sprites/Puerta2.png");
    qDebug() << "Puerta frame 1:" << !sprites[1].isNull();

    // Frame 2: Animación abriendo (más)
    sprites[2] = QPixmap(":/sprites/sprites/Puerta3.png");
    qDebug() << "Puerta frame 2:" << !sprites[2].isNull();

    // Frame 3: Puerta completamente abierta
    sprites[3] = QPixmap(":/sprites/sprites/Puerta4.png");
    qDebug() << "Puerta abierta:" << !sprites[3].isNull();
}

void Puerta::actualizar(float dt)
{
    if (!activo) return;

    // Actualizar animación de apertura
    if (animando)
    {
        tiempoAnimacion += dt;

        // Cambiar de frame cada velocidadAnimacion segundos
        if (tiempoAnimacion >= velocidadAnimacion)
        {
            tiempoAnimacion = 0.0f;
            frameActual++;

            qDebug() << "Puerta - frame:" << frameActual;

            // Si llegó al último frame, terminar animación
            if (frameActual >= 3)
            {
                frameActual = 3;
                animando = false;
                abierta = true;
                qDebug() << "¡Puerta completamente abierta!";
            }
        }
    }
}

void Puerta::renderizar(QPainter* painter)
{
    if (!activo || !painter) return;

    // Obtener sprite actual
    QPixmap spriteActual = sprites[frameActual];

    if (!spriteActual.isNull())
    {
        // Dibujar sprite de la puerta (ocupa 2 tiles de alto)
        painter->drawPixmap(x, y, ancho, alto, spriteActual);
    }
    else
    {
        // Fallback: Rectángulo de colores
        if (abierta || frameActual > 0)
        {
            // Puerta abierta/abriendo - verde
            painter->fillRect(x, y, ancho, alto, QColor(100, 200, 100));
        }
        else
        {
            // Puerta cerrada - marrón
            painter->fillRect(x, y, ancho, alto, QColor(139, 69, 19));
        }

        painter->setPen(QPen(Qt::black, 3));
        painter->drawRect(x, y, ancho, alto);

        // Manija
        painter->fillRect(x + 10, y + 60, 8, 8, Qt::yellow);
    }
}

void Puerta::interactuar(Jugador* jugador)
{
    if (abierta || animando) return;

    qDebug() << "¡Abriendo puerta!";
    abrir();
}

void Puerta::abrir()
{
    if (abierta || animando) return;

    animando = true;
    frameActual = 1;  // Empezar desde el primer frame de animación
    tiempoAnimacion = 0.0f;

    qDebug() << "Iniciando animación de apertura de puerta";
}
