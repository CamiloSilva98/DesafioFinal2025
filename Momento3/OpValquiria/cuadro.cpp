#include "cuadro.h"
#include "jugador.h"
#include <QPainter>
#include <QDebug>
#include <cmath>

Cuadro::Cuadro(float x, float y, int tileX, int tileY, ObjetoInteractivo* objetoOculto)
    : ObjetoInteractivo(x, y, 48, 48, TipoObjeto::PUERTA, "cuadro"),  // Usar PUERTA temporalmente
    quitado(false),
    tileX(tileX),
    tileY(tileY),
    objetoOculto(objetoOculto),
    tiempoAnimacion(0.0f),
    offsetY(0.0f)
{
    // Cargar sprites
    spriteCuadro = QPixmap(":/sprites/sprites/Cuadro.png");
    spriteVacio = QPixmap(":/sprites/sprites/ParedVacia.png");

    if (spriteCuadro.isNull()) {
        qDebug() << "Advertencia: No se pudo cargar sprite de cuadro";
    }
}

Cuadro::~Cuadro()
{
    // No eliminar objetoOculto aquí, lo maneja el nivel
}

void Cuadro::actualizar(float dt)
{
    if (!activo) return;

    // Animación de caída cuando se quita
    if (quitado && tiempoAnimacion < 1.0f) {
        tiempoAnimacion += dt * 2.0f;  // Velocidad de animación

        // Efecto de caída y desvanecimiento
        offsetY = tiempoAnimacion * 100.0f;  // Cae 100 píxeles

        if (tiempoAnimacion >= 1.0f) {
            activo = false;  // Desactivar después de la animación
        }
    }
}

void Cuadro::renderizar(QPainter* painter)
{
    if (!painter) return;

    if (quitado) {
        // Mostrar pared vacía
        if (!spriteVacio.isNull()) {
            painter->drawPixmap(x, y, ancho, alto, spriteVacio);
        } else {
            painter->fillRect(x, y, ancho, alto, QColor(95, 100, 110));
        }

        // Animación del cuadro cayendo
        if (tiempoAnimacion < 1.0f) {
            int alpha = 255 * (1.0f - tiempoAnimacion);
            painter->setOpacity(1.0f - tiempoAnimacion);

            if (!spriteCuadro.isNull()) {
                painter->drawPixmap(x, y + offsetY, ancho, alto, spriteCuadro);
            }

            painter->setOpacity(1.0f);  // Restaurar opacidad
        }

        return;
    }

    // Dibujar cuadro en la pared
    if (!spriteCuadro.isNull()) {
        painter->drawPixmap(x, y, ancho, alto, spriteCuadro);
    } else {
        // Fallback: Cuadro simple
        // Marco dorado
        painter->fillRect(x, y, ancho, alto, QColor(180, 140, 50));

        // Imagen (azul oscuro)
        painter->fillRect(x + 4, y + 4, ancho - 8, alto - 8, QColor(40, 60, 80));

        // Detalle del marco
        painter->setPen(QPen(QColor(220, 180, 80), 2));
        painter->drawRect(x + 2, y + 2, ancho - 4, alto - 4);
    }

    // Indicador de interacción si el jugador está cerca
    if (jugadorCerca(nullptr)) {  // Se verifica en el nivel
        painter->setPen(Qt::yellow);
        painter->setFont(QFont("Arial", 10, QFont::Bold));
        painter->drawText(x - 8, y - 8, "🖼️ E");

        // Brillo sutil
        painter->setBrush(QColor(255, 255, 0, 30));
        painter->setPen(Qt::NoPen);
        painter->drawRect(x - 2, y - 2, ancho + 4, alto + 4);
    }
}

void Cuadro::interactuar(Jugador* jugador)
{
    if (quitado || !activo) return;

    qDebug() << "¡Cuadro quitado de la pared!";
    quitado = true;
    tiempoAnimacion = 0.0f;

    // Revelar el objeto oculto
    if (objetoOculto) {
        objetoOculto->setActivo(true);
        qDebug() << "¡Objeto revelado detrás del cuadro!";
    }
}
