#ifndef JUGADOR_H
#define JUGADOR_H

#include "entidad.h"
#include <QKeyEvent>
#include <QPixmap>
#include <QVector>

enum class Direccion
{
    DOWN = 0,
    UP= 1,
    LEFT = 2,
    RIGTH = 3,
    ADOWN = 4,
    AUP = 5,
    ALEFT = 6,
    ARIGTH = 7

};

class Jugador : public Entidad
{
private:
    // Estado del jugador
    bool agachado;
    bool corriendo;
    bool oculto;
    bool muerto;
    float tiempoMuerte;
    Direccion direccionActual;
    Direccion direccionMuerte;

    // Velocidades
    static constexpr float VEL_NORMAL = 120.0f;
    static constexpr float VEL_CORRIENDO = 220.0f;
    static constexpr float VEL_AGACHADO = 60.0f;

    float hitboxOffsetY;
    float hitboxAlto;

    // Control de teclas
    bool teclaIzquierda;
    bool teclaDerecha;
    bool teclaArriba;
    bool teclaAbajo;
    bool teclaShift;
    bool teclaC;

    QVector<QVector<QPixmap>> sprites;

    static const int FRAMES_U = 4;
    static const int FRAMES_D = 4;
    static const int FRAMES_L = 4;
    static const int FRAMES_R = 4;
    static const int FRAMES_AU = 2;
    static const int FRAMES_AD = 2;
    static const int FRAMES_AL = 2;
    static const int FRAMES_AR = 2;


    QPixmap spriteMuerteU, spriteMuerteD, spriteMuerteL, spriteMuerteR;

    // Animación
    float tiempoAnimacion;
    int frameActual;

    QStringList nombresSpritesU, nombresSpritesD, nombresSpritesL, nombresSpritesR,
        nombresSpritesAU ,nombresSpritesAD ,nombresSpritesAL ,nombresSpritesAR;

public:
    Jugador(float x, float y);
    ~Jugador() override;

    void actualizar(float dt) override;
    void renderizar(QPainter* painter) override;

    void manejarTeclaPresionada(int key);
    void manejarTeclaSoltada(int key);

    // Getters
    bool estaAgachado() const { return agachado; }
    bool estaCorriendo() const { return corriendo; }
    bool estaOculto() const { return oculto; }
    bool estaVisible() const { return !oculto; }
    bool estaMuerto() const { return muerto; }

    void setOculto(bool valor) { oculto = valor; }
    void morir();
    void cargarSprites();

    float getHitboxY() const { return y + hitboxOffsetY; }
    float getHitboxAlto() const { return hitboxAlto;}

    int getNumFrames(Direccion dir) const;
    Direccion getDireccion() const { return direccionActual; }
};

#endif // JUGADOR_H
