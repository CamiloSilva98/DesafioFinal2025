#include "decoracion.h"
#include <QDebug>

Decoracion::Decoracion(float x, float y, float ancho, float alto,
                       TipoDecoracion tipo, bool solido)
    : x(x), y(y), ancho(ancho), alto(alto), tipo(tipo), solido(solido), activo(true)
{
    cargarSprite();
}

Decoracion::~Decoracion()
{
}

void Decoracion::cargarSprite()
{
    QString rutaSprite;

    switch (tipo)
    {
    case TipoDecoracion::ESCRITORIO:
        rutaSprite = ":/sprites/sprites/mesaB.png";
        break;
    case TipoDecoracion::SOFA:
        rutaSprite = ":/sprites/sprites/sofa.png";
        break;
    case TipoDecoracion::ESTANTERIA:
        rutaSprite = ":/sprites/sprites/librero.png";
        break;
    case TipoDecoracion::PLANTA:
        rutaSprite = ":/sprites/sprites/planta.png";
        break;
    case TipoDecoracion::ARCHIVADOR:
        rutaSprite = ":/sprites/sprites/librero2.png";
        break;
    case TipoDecoracion::MESA:
        rutaSprite = ":/sprites/sprites/table.png";
        break;
    case TipoDecoracion::CABALLETE:
        rutaSprite = ":/sprites/sprites/caballete.png";
        break;
    case TipoDecoracion::COFRE:
        rutaSprite = ":/sprites/sprites/cofre.png";
        break;
    case TipoDecoracion::CUADRO_DECORATIVO:
        rutaSprite = ":/sprites/sprites/AH.png";
        break;
    case TipoDecoracion::MESAL:
        rutaSprite = ":/sprites/sprites/Mesa.png";
    }

    sprite = QPixmap(rutaSprite);

    if (sprite.isNull()) {
        qDebug() << "Advertencia: No se pudo cargar sprite de decoración:" << rutaSprite;
    }
}

void Decoracion::renderizar(QPainter* painter)
{
    if (!activo || !painter) return;

    if (!sprite.isNull())
    {
        painter->drawPixmap(x, y, ancho, alto, sprite);
    }
    else
    {
    painter->fillRect(x, y, ancho, alto, QColor(150, 150, 150));
    }

// Debug: mostrar hitbox si es sólido
#ifdef DEBUG_COLISIONES
    if (solido) {
        painter->setPen(QPen(Qt::red, 1, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(x, y, ancho, alto);
    }
#endif
}

bool Decoracion::colisionaCon(float px, float py, float pAncho, float pAlto) const
{
    if (!solido || !activo) return false;

    return !(px + pAncho < x || px > x + ancho ||
             py + pAlto < y || py > y + alto);
}
