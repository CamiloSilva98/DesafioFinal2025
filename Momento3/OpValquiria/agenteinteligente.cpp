#include "agenteinteligente.h"
#include <QDebug>
//#include <Qsound>

AgenteInteligente::AgenteInteligente(float x, float y, float ancho, float alto)
    : Entidad(x, y, ancho, alto),
    estadoActual(EstadoAgente::PATRULLANDO),
    factorAprendizaje(1.0f)
{
}

AgenteInteligente::~AgenteInteligente()
{
}

void AgenteInteligente::cambiarEstado(EstadoAgente nuevoEstado)
{
    if (estadoActual != nuevoEstado)
    {
        qDebug() << "Agente cambió de estado:"
                 << (int)estadoActual << "->" << (int)nuevoEstado;
        estadoActual = nuevoEstado;
    }
    if (estadoActual != nuevoEstado)
    {
        qDebug() << "Guardia cambió de estado:"
                 << (int)estadoActual << "->" << (int)nuevoEstado;

        // ⭐ NUEVO: Reproducir sonidos
        if (nuevoEstado == EstadoAgente::PERSIGUIENDO)
        {
            //QSound::play(":/sound/sounds/freeze-2-86991.mp3");
        }

        estadoActual = nuevoEstado;
    }
}

