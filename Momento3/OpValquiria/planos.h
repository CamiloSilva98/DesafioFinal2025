#ifndef PLANOS_H
#define PLANOS_H

#include "objetointeractivo.h"
#include <QPixmap>

class Planos : public ObjetoInteractivo
{
private:
    QPixmap sprite;

public:
    Planos(float x, float y);
    ~Planos() override;

    void renderizar(QPainter* painter) override;
    void interactuar(Jugador* jugador) override;
};

#endif // PLANOS_H

