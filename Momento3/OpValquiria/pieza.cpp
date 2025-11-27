#include "pieza.h"
#include <QtMath>

Pieza::Pieza(TipoPieza t,
             const QPointF& posInit,
             const QPointF& posObj,
             float r)
    : tipo(t),
    posicion(posInit),
    posicionInicial(posInit),
    posicionObjetivo(posObj),
    radio(r),
    colocada(false),
    siendoArrastrada(false)
{
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
    // colisión circular entre posicion y posicionObjetivo
    qreal dx = posicion.x() - posicionObjetivo.x();
    qreal dy = posicion.y() - posicionObjetivo.y();
    qreal dist2 = dx*dx + dy*dy;
    return dist2 < (radio * radio);
}

bool Pieza::soltar() {
    siendoArrastrada = false;
    if (verificarEncaje()) {
        colocada = true;
        // la fijamos justo en la posición objetivo
        posicion = posicionObjetivo;
        return true;
    }
    return false;
}

void Pieza::regresarPosicionInicial() {
    if (!colocada) {
        posicion = posicionInicial;
        siendoArrastrada = false;
    }
}

void Pieza::dibujar(QPainter* painter) {
    painter->save();

    // Color según tipo
    QColor color;
    switch (tipo) {
    case TipoPieza::DETONADOR:      color = Qt::yellow; break;
    case TipoPieza::CARGA_EXPLOSIVA: color = Qt::red; break;
    case TipoPieza::TEMPORIZADOR:   color = Qt::cyan; break;
    }

    painter->setBrush(color);
    painter->setPen(Qt::black);

    // La dibujamos como un círculo simple
    painter->drawEllipse(posicion, 20, 20);

    painter->restore();
}

bool Pieza::contienePunto(const QPointF& p) const {
    qreal dx = p.x() - posicion.x();
    qreal dy = p.y() - posicion.y();
    qreal dist2 = dx*dx + dy*dy;
    return dist2 < (20.0 * 20.0); // mismo radio gráfico
}
