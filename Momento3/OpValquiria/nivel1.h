#ifndef NIVEL1_H
#define NIVEL1_H

#include "nivel.h"
#include <QPainter>
#include <QVector>


class Jugador;
class Guardia;
class ObjetoInteractivo;
class Planos;
class Llave;
class Decoracion;
class Cuadro;
class Puerta;

class Nivel1 : public Nivel {
private:
    // Mapa del nivel (tiles)
    static const int ANCHO_MAPA = 50;
    static const int ALTO_MAPA = 12;
    static const int TAMANO_TILE = 64;
    int mapa[ALTO_MAPA][ANCHO_MAPA];

    // Cámara (para hacer scroll horizontal)
    float camaraX;

    QPixmap tilePiso, tilePared, tileColumna, tilePuertaCerrada, tilePuertaAbierta, tileSombra;

    Jugador* jugador;
    QVector<Guardia*> guardias;

    QVector<ObjetoInteractivo*> objetos;
    Planos* planos1;
    Planos* planos2;
    Llave* llave;
    Puerta* puerta;

    QVector<Cuadro*> cuadros;
    QVector<Decoracion*> decoraciones;

    QPixmap spriteEscritorio, spriteSilla, spriteEstanteria;
    QPixmap spriteArchivador, spriteMesaReuniones;

    bool tienePlano1;
    bool tienePlano2;
    bool tieneLlave;
    bool puertaAbierta;
    QPointF posicionPuerta;
    QPointF posicionSalida;

    int detecciones;
    int MAX_DETECCIONES = 3;

    void crearMapa();
    void dibujarMapa(QPainter* painter);
    void actualizarCamara();
    void crearObjetos();
    void crearGuardias();
    void verificarDetecciones();
    void verificarInteracciones();
    void verificarCaptura();
    void abrirPuerta();
    void crearDecoraciones();
    void crearCuadros();

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
    void manejarTecla(QKeyEvent* event, bool pressed);

    bool hayColision(float x, float y, float ancho, float alto) const;
    int getTileEnPosicion(float x, float y) const;
    bool estaEnSombra(float x, float y) const;

};

#endif
