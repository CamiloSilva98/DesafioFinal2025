#ifndef OBJETOINTERACTIVO_H
#define OBJETOINTERACTIVO_H

#include "entidad.h"
#include <QString>

class Jugador;

enum class TipoObjeto {
    PLANOS,
    LLAVE,
    PUERTA
};

class ObjetoInteractivo : public Entidad
{
protected:
    bool recogido;
    bool interactuable;
    TipoObjeto tipo;
    QString id;  // Para identificar objetos específicos

public:
    ObjetoInteractivo(float x, float y, float ancho, float alto, TipoObjeto tipo, const QString& id = "");
    virtual ~ObjetoInteractivo();

    // Métodos virtuales
    virtual void actualizar(float dt) override;
    virtual void renderizar(QPainter* painter) override;
    virtual void interactuar(Jugador* jugador) = 0;

    // Getters
    bool estaRecogido() const { return recogido; }
    bool esInteractuable() const { return interactuable; }
    TipoObjeto getTipo() const { return tipo; }
    QString getId() const { return id; }

    // Verificar si el jugador está cerca
    bool jugadorCerca(Jugador* jugador) const;
};

#endif // OBJETOINTERACTIVO_H
