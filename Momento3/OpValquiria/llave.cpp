#include "llave.h"
#include <QPainter>
#include <QDebug>

Llave::Llave(float x, float y, const QString& idPuerta)
    : ObjetoInteractivo(x, y, 24, 24, TipoObjeto::LLAVE, "llave"),
    idPuerta(idPuerta)
{
    // Cargar sprite de llave
    sprite = QPixmap(":/sprites/sprites/llave.png");
    if (sprite.isNull()) {
        qDebug() << "Error: No se pudo cargar sprite de llave";
    }
}

Llave::~Llave()
{
}

void Llave::renderizar(QPainter* painter)
{
    if (!activo || recogido || !painter) return;

    if (!sprite.isNull())
    {
        painter->drawPixmap(x, y, ancho, alto, sprite);
    }
    else
    {
        // Fallback: Llave dorada
        painter->setBrush(QColor(255, 215, 0));
        painter->setPen(QPen(QColor(180, 150, 0), 2));

        // Cabeza de la llave (círculo)
        painter->drawEllipse(x, y, 12, 12);

        // Cuerpo de la llave
        painter->drawRect(x + 10, y + 5, 12, 3);

        // Dientes de la llave
        painter->drawLine(x + 18, y + 5, x + 18, y + 2);
        painter->drawLine(x + 22, y + 5, x + 22, y + 3);
    }
}

void Llave::interactuar(Jugador* jugador)
{
    if (!recogido && activo)
    {
        recogido = true;
        qDebug() << "¡Llave recogida! Abre la puerta:" << idPuerta;
    }
}
