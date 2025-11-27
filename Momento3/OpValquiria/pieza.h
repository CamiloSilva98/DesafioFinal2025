#ifndef PIEZA_H
#define PIEZA_H

#include <QPointF>
#include <QRectF>
#include <QPainter>

enum class TipoPieza {
    DETONADOR,
    CARGA_EXPLOSIVA,
    TEMPORIZADOR
};

class Pieza {
public:
    TipoPieza tipo;
    QPointF posicion;
    QPointF posicionInicial;
    QPointF posicionObjetivo;
    float radio;
    bool colocada;
    bool siendoArrastrada;

    Pieza(TipoPieza t,
          const QPointF& posInit,
          const QPointF& posObj,
          float r);

    void iniciarArrastre();
    void moverA(const QPointF& nuevaPos);
    bool soltar();                 // true si encaja
    bool verificarEncaje() const;
    void regresarPosicionInicial();

    void dibujar(QPainter* painter);
    bool contienePunto(const QPointF& p) const;
};

#endif
