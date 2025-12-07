#ifndef NIVEL2_H
#define NIVEL2_H

#include "nivel.h"
#include "pieza.h"
#include "maletin.h"

#include <vector>
#include <QPixmap>

class Nivel2 : public Nivel {
public:
    std::vector<Pieza*> piezas;
    Maletin*            maletin;

    int  tiempoRestante;
    int  errores;
    int  pasoActual;      // 0 = detonador, 1 = carga, 2 = temporizador
    Pieza* piezaSeleccionada;
    bool   mousePresionado;

    QPixmap libroSprite;
    QPixmap fondoSprite;
    bool    mostrarLibro;

    Nivel2();
    ~Nivel2() override;

    void inicializar() override;
    void actualizar(float dt) override;
    void renderizar(QPainter* painter) override;
    bool verificarCondicionVictoria() override;
    bool verificarCondicionDerrota() override;

    void manejarMousePress(const QPointF& pos);
    void manejarMouseMove(const QPointF& pos);
    void manejarMouseRelease(const QPointF& pos);

    void alternarLibro();

private:
    void dibujarLibro(QPainter* painter);
};

#endif // NIVEL2_H
