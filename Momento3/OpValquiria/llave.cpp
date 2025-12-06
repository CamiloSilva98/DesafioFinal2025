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
