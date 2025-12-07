#ifndef MALETIN_H
#define MALETIN_H

#include <QRectF>
#include <QPainter>
#include <QPixmap>

class Maletin {
public:
    QRectF  area;
    bool    cerrado;
    int     piezasNecesarias;
    int     piezasColocadas;

    QPixmap sprite;

    explicit Maletin(const QRectF& rect);

    void agregarPieza();
    bool estaCompleto() const;
    void cerrar();
    void dibujar(QPainter* painter);
};

#endif // MALETIN_H
