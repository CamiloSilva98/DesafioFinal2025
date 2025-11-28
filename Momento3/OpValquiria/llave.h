#ifndef LLAVE_H
#define LLAVE_H

#include "objetointeractivo.h"
#include <QPixmap>

class Llave : public ObjetoInteractivo
{
private:
    QPixmap sprite;
    QString idPuerta;  // ID de la puerta que abre

public:
    Llave(float x, float y, const QString& idPuerta);
    ~Llave() override;

    void renderizar(QPainter* painter) override;
    void interactuar(Jugador* jugador) override;

    QString getIdPuerta() const { return idPuerta; }
};

#endif // LLAVE_H
