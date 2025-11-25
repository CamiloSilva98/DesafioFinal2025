#ifndef ENTIDAD_H
#define ENTIDAD_H

#include <QPixmap>
#include <QPointF>

class QPainter;

class Entidad
{
protected:
    float x;
    float y;
    float velocidadX;
    float velocidadY;
    QPixmap sprite;
    bool activo;
    float ancho;
    float alto;

public:
    Entidad(float x = 0, float y = 0, float ancho = 32, float alto = 32);
    virtual ~Entidad();

    // Métodos virtuales puros (deben ser implementados por clases hijas)
    virtual void actualizar(float dt) = 0;
    virtual void renderizar(QPainter* painter) = 0;

    // Métodos comunes
    void mover(float dx, float dy);
    QPointF obtenerPosicion() const;
    bool colisionaCon(Entidad* otra) const;

    // Getters y Setters
    bool estaActivo() const { return activo; }
    void setActivo(bool valor) { activo = valor; }
    float getX() const { return x; }
    float getY() const { return y; }
    void setVelocidad(float vx, float vy);

    float getAncho() const { return ancho; }
    float getAlto() const { return alto; }
    void setX(float newX) { x = newX; }
    void setY(float newY) { y = newY; }
};

#endif
