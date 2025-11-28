#ifndef DECORACION_H
#define DECORACION_H

#include <QPixmap>
#include <QPainter>

enum class TipoDecoracion {
    ESCRITORIO,
    SILLA,
    ESTANTERIA,
    PLANTA,
    ARCHIVADOR,
    MESA,
    LAMPARA_PIE,
    MACETA,
    CUADRO_DECORATIVO  // Cuadros que solo decoran (no interactivos)
};

class Decoracion
{
private:
    float x, y;
    float ancho, alto;
    TipoDecoracion tipo;
    QPixmap sprite;
    bool solido;  // Si causa colisión o no
    bool activo;

public:
    Decoracion(float x, float y, float ancho, float alto,
               TipoDecoracion tipo, bool solido = false);
    ~Decoracion();

    void renderizar(QPainter* painter);
    void cargarSprite();

    // Getters
    float getX() const { return x; }
    float getY() const { return y; }
    float getAncho() const { return ancho; }
    float getAlto() const { return alto; }
    bool esSolido() const { return solido; }
    bool estaActivo() const { return activo; }

    // Verificar colisión rectangular
    bool colisionaCon(float px, float py, float pAncho, float pAlto) const;
};

#endif
