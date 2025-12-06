#ifndef CUADRO_H
#define CUADRO_H

#include "objetointeractivo.h"
#include <QPixmap>

class Cuadro : public ObjetoInteractivo
{
private:
    QPixmap spriteCuadro;
    bool quitado;
    bool recienQuitado;
    bool enPiso;
    int tileX;  // Posición en el mapa (tiles)
    int tileY;
    ObjetoInteractivo* objetoOculto;
    float tiempoRecienQuitado;


public:
    Cuadro(float x, float y, int tileX, int tileY, ObjetoInteractivo* objetoOculto);
    ~Cuadro() override;

    void actualizar(float dt) override;
    void renderizar(QPainter* painter) override;
    void interactuar(Jugador* jugador) override;

    bool estaQuitado() const { return quitado; }
    bool bloqueaInteraccionObjeto() const;
    int getTileX() const { return tileX; }
    int getTileY() const { return tileY; }
    ObjetoInteractivo* getObjetoOculto() { return objetoOculto; }
    bool jugadorCerca(Jugador* jugador) const;
};

#endif // CUADRO_H
