#include "objetointeractivo.h"
#include "jugador.h"
#include <cmath>
#include <QDebug>

ObjetoInteractivo::ObjetoInteractivo(float x, float y, float ancho, float alto,
                                     TipoObjeto tipo, const QString& id)
    : Entidad(x, y, ancho, alto),
    recogido(false),
    interactuable(true),
    tipo(tipo),
    id(id)
{
}

ObjetoInteractivo::~ObjetoInteractivo()
{
}

void ObjetoInteractivo::actualizar(float dt)
{
    // Los objetos no se mueven por defecto
}

void ObjetoInteractivo::renderizar(QPainter* painter)
{
    // Implementación por defecto (se sobrescribe en clases hijas)
}

bool ObjetoInteractivo::jugadorCerca(Jugador* jugador) const
{
    if (!jugador) return false;

    float distancia = std::sqrt(
        std::pow(jugador->getX() - x, 2) +
        std::pow(jugador->getY() - y, 2)
        );

    return distancia < 50.0f;  // Dentro de 50 píxeles
}
