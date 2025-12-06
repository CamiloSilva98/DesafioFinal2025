#include "planos.h"
#include "jugador.h"
#include <QPainter>
#include <QDebug>

Planos::Planos(float x, float y)
    : ObjetoInteractivo(x, y, 32, 32, TipoObjeto::PLANOS, "planos")
{
    // Cargar sprite de planos
    sprite = QPixmap(":/sprites/sprites/plano.png");
    if (sprite.isNull()) {
        qDebug() << "Error: No se pudo cargar sprite de planos";
    }
}

Planos::~Planos()
{
}

void Planos::renderizar(QPainter* painter)
{
    if (!activo || recogido || !painter) return;

    if (!sprite.isNull())
    {
        painter->drawPixmap(x, y, ancho, alto, sprite);
    }
    else
    {
        // Fallback: Papel amarillo
        painter->fillRect(x, y, ancho, alto, QColor(255, 255, 150));
        painter->setPen(QPen(Qt::black, 2));
        painter->drawRect(x, y, ancho, alto);

        // Líneas simulando texto
        painter->setPen(QPen(Qt::black, 1));
        for (int i = 0; i < 3; i++) {
            painter->drawLine(x + 5, y + 8 + i*8, x + ancho - 5, y + 8 + i*8);
        }
    }
}

void Planos::interactuar(Jugador* jugador)
{
    if (!recogido && activo)
    {
        recogido = true;
        qDebug() << "¡Planos recogidos!";
    }
}

