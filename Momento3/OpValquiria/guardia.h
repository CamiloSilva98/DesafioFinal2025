#ifndef GUARDIA_H
#define GUARDIA_H

#include "agenteinteligente.h"
#include <QVector>
#include <QPointF>
#include <QPixmap>

class Jugador;
class Nivel1;

enum class DireccionGuardia
{
    ARRIBA = 0,
    ABAJO = 1,
    IZQUIERDA = 2,
    DERECHA = 3
};

class Guardia : public AgenteInteligente
{
private:
    // Ruta de patrullaje
    QVector<QPointF> rutaPatrullaje;
    int puntoActual;
    bool avanzandoEnRuta;  // true = hacia adelante, false = hacia atrás

    // Sensores
    float campoVision;      // Ángulo de visión (en grados)
    float rangoVision;      // Distancia máxima de visión
    float rangoAuditivo;    // Distancia para escuchar ruidos
    float direccionVista;   // Ángulo actual de visión (0° = derecha)

    // Memoria
    QPointF ultimaPosicionJugador;
    bool jugadorVisto;
    float tiempoBusqueda;
    float tiempoMaximoBusqueda;

    // Aprendizaje
    QVector<QPointF> zonasDeteccion;  // Lugares donde detectó al jugador

    // Velocidades
    static constexpr float VEL_PATRULLA = 30.0f;
    static constexpr float VEL_INVESTIGAR = 80.0f;
    static constexpr float VEL_PERSEGUIR = 110.0f;

    // Referencia al jugador (para detectarlo)
    Jugador* jugadorObjetivo;
    Nivel1* nivelRef;

    // Sprites
    QVector<QVector<QPixmap>> sprites;  // sprites[direccion][frame]
    DireccionGuardia direccionActual;
    int frameActual;
    float tiempoAnimacion;
    static const int FRAMES_LATERAL = 2;
    static const int FRAMES_VERTICAL = 3;

public:
    Guardia(float x, float y, const QVector<QPointF>& ruta);
    ~Guardia() override;

    // Ciclo de IA (heredado)
    void percibir() override;
    void razonar() override;
    void actuar(float dt) override;
    void aprenderDeExperiencia() override;

    // Implementación de Entidad
    void actualizar(float dt) override;
    void renderizar(QPainter* painter) override;

    // Comportamientos específicos
    void patrullar(float dt);
    void investigar(float dt, QPointF objetivo);
    void perseguir(float dt);
    void buscar(float dt);
    void actualizarDireccionVista(float dx, float dy);
    void setNivel(Nivel1* nivel) { nivelRef = nivel; }
    void intentarRodearObstaculo(QPointF objetivo, float dt);
    int contarParedesEnLinea(QPointF desde, QPointF hasta);

    // Detección
    bool detectarJugador();
    bool detectarRuido();
    void rotarVision(float dt);

    // Configuración
    void setJugadorObjetivo(Jugador* jugador) { jugadorObjetivo = jugador; }
    void cargarSprites();

    // Getters
    bool haDetectadoJugador() const { return jugadorVisto; }
    QPointF getUltimaPosicionJugador() const { return ultimaPosicionJugador; }
    int getNumFrames(DireccionGuardia dir) const;

private:
    // Métodos auxiliares
    float calcularDistanciaAJugador() const;
    float calcularAnguloAJugador() const;
    bool estaEnCampoVision(float anguloAJugador) const;
    void moverHaciaPunto(QPointF objetivo, float velocidad, float dt);
    void actualizarDireccionSprite(float dx, float dy);
    bool verificarColision(float newX, float newY);
    bool hayLineaDeVista(QPointF desde, QPointF hasta);
    bool intentandoRodear;
    QPointF puntoRodeo;
    float tiempoAtascado;
};

#endif // GUARDIA_H
