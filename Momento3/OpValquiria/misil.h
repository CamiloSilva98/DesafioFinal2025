#ifndef MISIL_H
#define MISIL_H

#include "agenteinteligente.h"
#include <QPointF>

class Avion;

enum class EstadoMisil {
    BUSQUEDA,
    PERSECUCION,
    PREDICCION,
    EXPLOSION
};

class Misil : public AgenteInteligente
{
private:
    // Objetivo
    Avion* objetivo;

    // Predicción
    float leadFactor;
    int fallosConsecutivos;
    QPointF posicionPredicha;

    // Velocidades
    float velocidadMisil;
    static constexpr float VEL_BASE = 180.0f;
    static constexpr float VEL_MAX = 250.0f;

    // Estado
    EstadoMisil estadoMisil;
    float tiempoVida;
    float tiempoMaximoVida;
    bool detonado;

    // Animación explosión
    float tiempoExplosion;
    float radioExplosion;

    // Sprites
    QPixmap spriteMisil;
    float rotacion;  // Ángulo de rotación del sprite

public:
    Misil(float x, float y, Avion* objetivo);
    ~Misil() override;

    // Ciclo de IA (heredado)
    void percibir() override;
    void razonar() override;
    void actuar(float dt) override;
    void aprenderDeExperiencia() override;

    // Implementación de Entidad
    void actualizar(float dt) override;
    void renderizar(QPainter* painter) override;

    // Comportamientos
    void buscar();
    void perseguir();
    void predecir();
    void explotar();

    // Cálculos
    QPointF calcularPosicionFutura();
    void actualizarRotacion();

    // Getters
    bool debeEliminarse() const { return !activo || tiempoVida >= tiempoMaximoVida || detonado; }
    bool estaDetonado() const { return detonado; }
    float getLeadFactor() const { return leadFactor; }

private:
    void cargarSprite();
    float calcularDistanciaAObjetivo() const;
    QPointF calcularDireccionAObjetivo(QPointF puntoObjetivo);
};

#endif // MISIL_H
