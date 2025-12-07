#ifndef OBSTACULO_H
#define OBSTACULO_H

#include "entidad.h"
#include <QPixmap>

enum class TipoObstaculo {
    // Fase terrestre
    VALLA,          // Se salta
    BARRERA_BAJA,   // Se agacha
    GUARDIA,        // Se evita

    // Fase aérea
    NUBE,           // Decorativo
    TORRE,          // Sólido
    GLOBO,          // Sólido
    AVION_ENEMIGO   // Sólido
};

class Obstaculo : public Entidad
{
private:
    TipoObstaculo tipo;
    float velocidadScroll;
    bool solido;  // Si causa colisión
    QPixmap sprite;

public:
    Obstaculo(float x, float y, TipoObstaculo tipo, float velocidadScroll = 50.0f);
    ~Obstaculo() override;

    void actualizar(float dt) override;
    void renderizar(QPainter* painter) override;

    // Getters
    TipoObstaculo getTipo() const { return tipo; }
    bool esSolido() const { return solido; }
    bool fueraDePantalla() const { return x < -ancho; }

private:
    void cargarSprite();
};

#endif // OBSTACULO_H
