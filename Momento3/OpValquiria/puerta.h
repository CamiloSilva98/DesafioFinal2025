#ifndef PUERTA_H
#define PUERTA_H

#include "objetointeractivo.h"
#include <QPixmap>
#include <QVector>

class Puerta : public ObjetoInteractivo
{
private:
    bool abierta;
    bool animando;
    int frameActual;
    float tiempoAnimacion;
    float velocidadAnimacion;

    QVector<QPixmap> sprites;  // 4 sprites: cerrada, frame1, frame2, abierta

public:
    Puerta(float x, float y);
    ~Puerta() override;

    void actualizar(float dt) override;
    void renderizar(QPainter* painter) override;
    void interactuar(Jugador* jugador) override;

    void abrir();
    bool estaAbierta() const { return abierta; }
    bool estaAnimando() const { return animando; }

private:
    void cargarSprites();
};

#endif // PUERTA_H
