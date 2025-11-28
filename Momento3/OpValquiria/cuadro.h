#ifndef CUADRO_H
#define CUADRO_H

#include "objetointeractivo.h"
#include <QPixmap>

class Cuadro : public ObjetoInteractivo
{
private:
    QPixmap spriteCuadro;
    QPixmap spriteVacio;  // Pared vacía después de quitar el cuadro
    bool quitado;
    int tileX;  // Posición en el mapa (tiles)
    int tileY;
    ObjetoInteractivo* objetoOculto;  // Llave o plano detrás

    // Animación
    float tiempoAnimacion;
    float offsetY;  // Para efecto de "caída" al quitar

public:
    Cuadro(float x, float y, int tileX, int tileY, ObjetoInteractivo* objetoOculto);
    ~Cuadro() override;

    void actualizar(float dt) override;
    void renderizar(QPainter* painter) override;
    void interactuar(Jugador* jugador) override;

    bool estaQuitado() const { return quitado; }
    int getTileX() const { return tileX; }
    int getTileY() const { return tileY; }
    ObjetoInteractivo* getObjetoOculto() { return objetoOculto; }
};

#endif // CUADRO_H
