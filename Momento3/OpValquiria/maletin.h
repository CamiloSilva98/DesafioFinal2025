#ifndef MALETIN_H
#define MALETIN_H

#include <QRectF>
#include <QPainter>

class Maletin {
public:
    QRectF area;
    bool cerrado;
    int piezasNecesarias;
    int piezasColocadas;

    Maletin(const QRectF& rect);

    void agregarPieza();
    bool estaCompleto() const;
    void cerrar();
    void dibujar(QPainter* painter);
};

#endif
