#ifndef NIVEL3_H
#define NIVEL3_H

#include "nivel.h"
#include <QPainter>
#include <QVector>
#include <QPointF>

class Jugador;
class Avion;
class Misil;
class Obstaculo;

enum class Fase {
    TERRESTRE,
    AEREO,
    COMPLETADO
};


class Nivel3 : public Nivel {
private:
    // Control de fases
    Fase faseActual;
    float tiempoFase;
    float tiempoLimite;

    // FASE 1: Terrestre
    Jugador* jugador;
    QVector<Obstaculo*> obstaculosTerrestre;
    float scrollX;  // Posición del scroll
    float velocidadBase;
    bool enSalto;
    float velocidadSaltoY;
    float tiempoSalto;

    // FASE 2: Aérea
    Avion* avion;
    QVector<Misil*> misiles;
    QVector<Obstaculo*> obstaculosAereos;
    float tiempoGeneracionMisil;
    int misilesEsquivados;

    // Gráficos
    QPixmap fondoCielo;
    QPixmap fondoPista;
    QPixmap fondoNubes;
    float offsetFondo1;  // Para parallax
    float offsetFondo2;

    // Estado del nivel
    int choques;
    bool llegandoAvion;

    // Constantes
    static const int ANCHO_NIVEL = 5000;
    static const int ALTO_NIVEL = 600;
    static const int MAX_CHOQUES = 3;
    static constexpr float GRAVEDAD = 980.0f;  // px/s²

    // Métodos privados
    void actualizarFaseTerrestre(float dt);
    void actualizarFaseAerea(float dt);
    void renderizarFaseTerrestre(QPainter* painter);
    void renderizarFaseAerea(QPainter* painter);

    void generarObstaculosTerrestre();
    void generarObstaculosAereos();
    void generarMisil();

    void aplicarGravedad(float dt);
    void manejarSalto();
    void verificarColisionesTerrestre();
    void verificarColisionesAereas();

    void cambiarFase();
    void actualizarScrolling(float dt);
    void actualizarParallax(float dt);

public:
    Nivel3();
    ~Nivel3() override;

    void inicializar() override;
    void actualizar(float dt) override;
    void renderizar(QPainter* painter) override;
    bool verificarCondicionVictoria() override;
    bool verificarCondicionDerrota() override;

    void manejarTecla(QKeyEvent* event, bool pressed);

    // Getters
    Fase getFaseActual() const { return faseActual; }
    int getMisilesEsquivados() const { return misilesEsquivados; }
};

#endif // NIVEL3_H
