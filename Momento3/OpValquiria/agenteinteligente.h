#ifndef AGENTEINTELIGENTE_H
#define AGENTEINTELIGENTE_H

#include "entidad.h"

enum class EstadoAgente {
    PATRULLANDO,
    INVESTIGANDO,
    PERSIGUIENDO,
    BUSCANDO
};

class AgenteInteligente : public Entidad
{
protected:
    EstadoAgente estadoActual;
    float factorAprendizaje;

public:
    AgenteInteligente(float x = 0, float y = 0, float ancho = 32, float alto = 48);
    virtual ~AgenteInteligente();

    // Métodos virtuales puros - Ciclo de IA
    virtual void percibir() = 0;
    virtual void razonar() = 0;
    virtual void actuar(float dt) = 0;
    virtual void aprenderDeExperiencia() = 0;

    // Métodos comunes
    void cambiarEstado(EstadoAgente nuevoEstado);
    EstadoAgente getEstado() const { return estadoActual; }
};

#endif // AGENTEINTELIGENTE_H
