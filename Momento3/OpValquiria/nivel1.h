#ifndef NIVEL1_H
#define NIVEL1_H

#include "nivel.h"
#include <QPainter>

    class Nivel1 : public Nivel {
private:
    // Mapa del nivel (tiles)
    static const int ANCHO_MAPA = 50;  // 50 tiles de ancho
    static const int ALTO_MAPA = 12;   // 12 tiles de alto
    static const int TAMANO_TILE = 64;
    int mapa[ALTO_MAPA][ANCHO_MAPA];

    // Cámara (para hacer scroll horizontal)
    float camaraX;

public:
    Nivel1();
    ~Nivel1() override;

    void cargarSprites();
    void inicializar() override;
    void actualizar(float dt) override;
    void renderizar(QPainter* painter) override;
    bool verificarCondicionVictoria() override;
    bool verificarCondicionDerrota() override;

    void moverCamara(float dx);  // Para probar el scroll

private:
    void crearMapa();
    QPixmap tilePiso, tilePared, tileCaja, tilePuertaCerrada, tilePuertaAbierta, tileVentana, tileLampara, tileSombra;
    void dibujarMapa(QPainter* painter);
};

#endif
