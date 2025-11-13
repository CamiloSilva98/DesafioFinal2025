#include "entidad.h"
#include <QPainter>
#include <cmath>

Entidad::Entidad(float x, float y, float ancho, float alto)
    : x(x), y(y), ancho(ancho), alto(alto),
    velocidadX(0), velocidadY(0), activo(true)
{
}

Entidad::~Entidad() {
}

void Entidad::mover(float dx, float dy) {
    x += dx;
    y += dy;
}

QPointF Entidad::obtenerPosicion() const {
    return QPointF(x, y);
}

bool Entidad::colisionaCon(Entidad* otra) const {
    if (!activo || !otra->estaActivo()) {
        return false;
    }

    // Colisión rectangular simple
    float izq1 = x;
    float der1 = x + ancho;
    float arr1 = y;
    float aba1 = y + alto;

    float izq2 = otra->x;
    float der2 = otra->x + otra->ancho;
    float arr2 = otra->y;
    float aba2 = otra->y + otra->alto;

    return !(der1 < izq2 || der2 < izq1 || aba1 < arr2 || aba2 < arr1);
}

void Entidad::setVelocidad(float vx, float vy) {
    velocidadX = vx;
    velocidadY = vy;
}
