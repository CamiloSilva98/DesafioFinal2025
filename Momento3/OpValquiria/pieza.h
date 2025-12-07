#ifndef PIEZA_H
#define PIEZA_H

#include <QPointF>
#include <QRectF>
#include <QPainter>
#include <QPixmap>

enum class TipoPieza {
    DETONADOR,
    CARGA_EXPLOSIVA,
    TEMPORIZADOR,
    RANDOM1,
    RANDOM2,
    RANDOM3,
    RANDOM4,
    RANDOM5,
    RANDOM6,
    RANDOM7,
    RANDOM8,
    RANDOM9
};

class Pieza {
public:
    TipoPieza tipo;
    QPointF posicion;
    QPointF posicionInicial;
    QPointF posicionObjetivo;
    float   radio;
    bool    colocada;
    bool    siendoArrastrada;
    bool    esCorrecta;      // true si es detonador, carga o temporizador

    QPixmap sprite;

    Pieza(TipoPieza t,
          const QPointF& posInit,
          const QPointF& posObj,
          float r,
          bool correcta);

    void iniciarArrastre();
    void moverA(const QPointF& nuevaPos);
    bool soltar();
    bool verificarEncaje() const;
    void regresarPosicionInicial();

    void dibujar(QPainter* painter);
    bool contienePunto(const QPointF& p) const;
};

#endif // PIEZA_H
