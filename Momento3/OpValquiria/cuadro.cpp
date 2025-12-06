#include "cuadro.h"
#include "jugador.h"
#include <QPainter>
#include <QDebug>
#include <cmath>

Cuadro::Cuadro(float x, float y, int tileX, int tileY, ObjetoInteractivo* objetoOculto)
    : ObjetoInteractivo(x, y, 50, 55, TipoObjeto::LLAVE, "cuadro"),
    quitado(false),
    recienQuitado(false),
    enPiso(false),
    tileX(tileX),
    tileY(tileY),
    objetoOculto(objetoOculto),
    tiempoRecienQuitado(0.0f)
{
    // Cargar sprites
    spriteCuadro = QPixmap(":/sprites/sprites/AH.png");

    if (spriteCuadro.isNull())
    {
        qDebug() << "Advertencia: No se pudo cargar sprite de cuadro";
    }
}
Cuadro::~Cuadro()
{}
void Cuadro::renderizar(QPainter* painter)
{
    if (!painter) return;

    // Si está quitado, mostrar en el piso
    if (quitado)
    {
        float pisoY = y + 25;
        float pisoX = x - 35;

        if (!spriteCuadro.isNull()) {
            painter->drawPixmap(pisoX, pisoY, ancho, alto, spriteCuadro);
        } else {
            // Fallback
            painter->fillRect(pisoX, pisoY, ancho, alto, QColor(150, 120, 50));
        }

        return;
    }

    // Dibujar cuadro colgado en la pared
    if (!spriteCuadro.isNull()) {
        painter->drawPixmap(x, y, ancho, alto, spriteCuadro);
    } else {
        // Fallback: Marco dorado
        painter->fillRect(x, y, ancho, alto, QColor(180, 140, 50));
        painter->fillRect(x + 4, y + 4, ancho - 8, alto - 8, QColor(40, 60, 80));
        painter->setPen(QPen(QColor(220, 180, 80), 2));
        painter->drawRect(x + 2, y + 2, ancho - 4, alto - 4);
    }
}

void Cuadro::interactuar(Jugador* jugador)
{
    if (quitado || !activo) return;

    qDebug() << "¡Cuadro quitado de la pared!";
    quitado = true;
    recienQuitado = true;
    tiempoRecienQuitado = 0.0f;

    // Revelar el objeto oculto
    if (objetoOculto)
    {
        objetoOculto->setActivo(true);
        qDebug() << "¡Objeto revelado detrás del cuadro!";
    }

}
bool Cuadro::bloqueaInteraccionObjeto() const
{
    return recienQuitado;  // ✅ Bloquear si fue recién quitado
}
bool Cuadro::jugadorCerca(Jugador* jugador) const
{
    if (!jugador) return false;

    float distancia = std::sqrt(
        std::pow(jugador->getX() + jugador->getAncho()/2 - (x + ancho/2), 2) +
        std::pow(jugador->getY() + jugador->getAlto()/2 - (y + alto/2), 2)
        );

    return distancia < 70.0f;  // 70 píxeles de rango
}
void Cuadro::actualizar(float dt)
{
    if (recienQuitado) {
        tiempoRecienQuitado += dt;

        // ✅ DEBUG: Mostrar progreso cada frame
        static int frameCount = 0;
        if (frameCount++ % 30 == 0) {  // Cada medio segundo aprox
            qDebug() << "Cuadro actualizando - Tiempo:" << tiempoRecienQuitado
                     << "/ 0.3s | Bloqueado:" << recienQuitado;
        }

        if (tiempoRecienQuitado >= 0.3f)
        {
            recienQuitado = false;
            qDebug() << "✓✓✓ BLOQUEO LIBERADO - Objeto disponible para recoger";
        }
    }
}

