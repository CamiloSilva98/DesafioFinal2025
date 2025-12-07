#ifndef AVION_H
#define AVION_H

#include "entidad.h"

class Avion : public Entidad
{
private:
    // Movimiento
    float velocidadBase;
    float aceleracion;

    // Oscilación natural (movimiento sinusoidal)
    float amplitudOscilacion;
    float frecuenciaOscilacion;
    float tiempoOscilacion;

    // Turbo
    bool turboActivo;
    float tiempoTurbo;
    float tiempoRecargaTurbo;
    float duracionTurbo;

    // Evasión
    bool enEvasion;
    float tiempoEvasion;

    // Sprites
    QVector<QPixmap> sprites;  // Normal, inclinado arriba, inclinado abajo
    int spriteActual;

    // Límites de movimiento
    float limiteArriba;
    float limiteAbajo;

    // Constantes
    static constexpr float VEL_BASE = 100.0f;
    static constexpr float VEL_TURBO = 200.0f;
    static constexpr float VEL_VERTICAL = 150.0f;

public:
    Avion(float x, float y);
    ~Avion() override;

    void actualizar(float dt) override;
    void renderizar(QPainter* painter) override;

    // Controles
    void moverArriba(float dt);
    void moverAbajo(float dt);
    void acelerar();
    void frenar();
    void evasion();
    void activarTurbo();

    // Física
    void aplicarOscilacion(float dt);

    // Getters
    bool tieneTurboDisponible() const { return tiempoRecargaTurbo >= duracionTurbo; }
    bool estaTurboActivo() const { return turboActivo; }
    float getVelocidadActual() const;

private:
    void cargarSprites();
    void actualizarSprite();
};

#endif // AVION_H
