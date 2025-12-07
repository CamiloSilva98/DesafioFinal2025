#include "pieza.h"

#include <QtMath>
#include <QDebug>

Pieza::Pieza(TipoPieza t,
             const QPointF& posInit,
             const QPointF& posObj,
             float r,
             bool correcta)
    : tipo(t),
    posicion(posInit),
    posicionInicial(posInit),
    posicionObjetivo(posObj),
    radio(r),
    colocada(false),
    siendoArrastrada(false),
    esCorrecta(correcta)
{
    // Cargar sprite según tipo
    switch (tipo) {
    case TipoPieza::DETONADOR:
        sprite.load(":/img/sprites/detonador.png");
        break;
    case TipoPieza::CARGA_EXPLOSIVA:
        sprite.load(":/img/sprites/explosivo.png");
        break;
    case TipoPieza::TEMPORIZADOR:
        sprite.load(":/img/sprites/temporizador.png");
        break;
    case TipoPieza::RANDOM1:
        sprite.load(":/img/sprites/random1.png");
        break;
    case TipoPieza::RANDOM2:
        sprite.load(":/img/sprites/random2.png");
        break;
    case TipoPieza::RANDOM3:
        sprite.load(":/img/sprites/random3.png");
        break;
    case TipoPieza::RANDOM4:
        sprite.load(":/img/sprites/random4.png");
        break;
    case TipoPieza::RANDOM5:
        sprite.load(":/img/sprites/random5.png");
        break;
    case TipoPieza::RANDOM6:
        sprite.load(":/img/sprites/random6.png");
        break;
    case TipoPieza::RANDOM7:
        sprite.load(":/img/sprites/random7.png");
        break;
    case TipoPieza::RANDOM8:
        sprite.load(":/img/sprites/random8.png");
        break;
    case TipoPieza::RANDOM9:
        sprite.load(":/img/sprites/random9.png");
        break;
    }

    if (sprite.isNull()) {
        qDebug() << "ERROR: No se pudo cargar sprite para pieza tipo" << static_cast<int>(tipo);
    }
}

void Pieza::iniciarArrastre() {
    if (!colocada) {
        siendoArrastrada = true;
    }
}

void Pieza::moverA(const QPointF& nuevaPos) {
    if (!colocada && siendoArrastrada) {
        posicion = nuevaPos;
    }
}

bool Pieza::verificarEncaje() const {
    const qreal dx   = posicion.x() - posicionObjetivo.x();
    const qreal dy   = posicion.y() - posicionObjetivo.y();
    const qreal dist = qSqrt(dx * dx + dy * dy);
    return dist <= radio;
}

bool Pieza::soltar() {
    siendoArrastrada = false;
    if (verificarEncaje()) {
        colocada  = true;
        posicion  = posicionObjetivo;
        return true;
    }
    return false;
}

void Pieza::regresarPosicionInicial() {
    if (!colocada) {
        posicion         = posicionInicial;
        siendoArrastrada = false;
    }
}

void Pieza::dibujar(QPainter* painter) {
    painter->save();

    if (!sprite.isNull()) {
        const int w = 80;  // Tamaño ajustado
        const int h = 80;
        const QRectF rect(posicion.x() - w / 2.0,
                          posicion.y() - h / 2.0,
                          w, h);
        painter->drawPixmap(rect.toRect(), sprite);
    } else {
        // Fallback si no carga sprite
        QColor color = Qt::gray;

        if (esCorrecta) {
            switch (tipo) {
            case TipoPieza::DETONADOR:
                color = Qt::yellow;
                break;
            case TipoPieza::CARGA_EXPLOSIVA:
                color = Qt::red;
                break;
            case TipoPieza::TEMPORIZADOR:
                color = Qt::cyan;
                break;
            default:
                break;
            }
        }

        painter->setBrush(color);
        painter->setPen(Qt::black);
        painter->drawEllipse(posicion, 30.0, 30.0);
    }

    painter->restore();
}

bool Pieza::contienePunto(const QPointF& p) const {
    const qreal dx   = p.x() - posicion.x();
    const qreal dy   = p.y() - posicion.y();
    const qreal dist = qSqrt(dx * dx + dy * dy);
    return dist <= 40.0;  // Radio de detección de clic
}
