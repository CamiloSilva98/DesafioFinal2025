#include "avion.h"
#include <QPainter>
#include <QDebug>
#include <cmath>

Avion::Avion(float x, float y)
    : Entidad(x, y, 80, 40),  // 80x40 píxeles
    velocidadBase(VEL_BASE),
    aceleracion(0.0f),
    amplitudOscilacion(10.0f),
    frecuenciaOscilacion(2.0f * M_PI / 3.0f),  // ω = 2π/3 rad/s
    tiempoOscilacion(0.0f),
    turboActivo(false),
    tiempoTurbo(0.0f),
    tiempoRecargaTurbo(3.0f),
    duracionTurbo(3.0f),
    enEvasion(false),
    tiempoEvasion(0.0f),
    spriteActual(0),
    limiteArriba(50.0f),
    limiteAbajo(650.0f)
{
    sprites.resize(3);
    cargarSprites();

    qDebug() << "Avión creado en posición:" << x << "," << y;
}

Avion::~Avion()
{
}

void Avion::cargarSprites()
{
    qDebug() << "=== CARGANDO SPRITES DEL AVIÓN ===";

    // Sprite normal (horizontal)
    sprites[0] = QPixmap(":/sprites/sprites/avion.png");

    // Sprite inclinado arriba
    sprites[1] = QPixmap(":/sprites/sprites/avion_arriba.png");

    // Sprite inclinado abajo
    sprites[2] = QPixmap(":/sprites/sprites/avion_abajo.png");

    // Verificar carga
    for (int i = 0; i < sprites.size(); i++) {
        qDebug() << "Sprite" << i << ":" << !sprites[i].isNull();
    }

    qDebug() << "=== FIN CARGA SPRITES AVIÓN ===";
}

void Avion::actualizar(float dt)
{
    if (!activo) return;

    // Actualizar tiempo de recarga de turbo
    if (!turboActivo && tiempoRecargaTurbo < duracionTurbo) {
        tiempoRecargaTurbo += dt;
    }

    // Actualizar turbo
    if (turboActivo) {
        tiempoTurbo += dt;

        if (tiempoTurbo >= 2.0f) {  // Turbo dura 2 segundos
            turboActivo = false;
            tiempoTurbo = 0.0f;
            tiempoRecargaTurbo = 0.0f;
            velocidadBase = VEL_BASE;
            qDebug() << "Turbo agotado";
        }
    }

    // Actualizar evasión
    if (enEvasion) {
        tiempoEvasion += dt;

        if (tiempoEvasion >= 0.5f) {  // Evasión dura 0.5 segundos
            enEvasion = false;
            tiempoEvasion = 0.0f;
        }
    }

    // Limitar posición vertical
    if (y < limiteArriba) y = limiteArriba;
    if (y > limiteAbajo) y = limiteAbajo;

    // Actualizar sprite según movimiento
    actualizarSprite();
}

void Avion::renderizar(QPainter* painter)
{
    if (!activo || !painter) return;

    QPixmap spriteUsar;

    if (spriteActual >= 0 && spriteActual < sprites.size()) {
        spriteUsar = sprites[spriteActual];
    }

    if (!spriteUsar.isNull()) {
        // Efecto visual de turbo
        if (turboActivo) {
            painter->setOpacity(0.8f + 0.2f * std::sin(tiempoTurbo * 10.0f));
        }

        painter->drawPixmap(x, y, ancho, alto, spriteUsar);

        painter->setOpacity(1.0f);

        // Llama del turbo
        if (turboActivo) {
            painter->setBrush(QColor(255, 100, 0, 150));
            painter->setPen(Qt::NoPen);
            painter->drawEllipse(x - 20, y + 15, 20, 10);
        }
    } else {
        // Fallback: Triángulo simple
        painter->setBrush(QColor(100, 100, 200));
        painter->setPen(QPen(Qt::black, 2));

        QPolygonF triangulo;
        triangulo << QPointF(x + ancho, y + alto/2)  // Punta
                  << QPointF(x, y)                    // Arriba atrás
                  << QPointF(x, y + alto);            // Abajo atrás

        painter->drawPolygon(triangulo);

        // Alas
        painter->drawLine(x + ancho/2, y + alto/2, x + ancho/2 - 15, y - 10);
        painter->drawLine(x + ancho/2, y + alto/2, x + ancho/2 - 15, y + alto + 10);
    }

// Debug: Hitbox
#ifdef DEBUG_COLISIONES
    painter->setPen(QPen(Qt::red, 1, Qt::DashLine));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(x, y, ancho, alto);
#endif
}

void Avion::moverArriba(float dt)
{
    velocidadY = -VEL_VERTICAL;
    y += velocidadY * dt;

    if (y < limiteArriba) y = limiteArriba;
}

void Avion::moverAbajo(float dt)
{
    velocidadY = VEL_VERTICAL;
    y += velocidadY * dt;

    if (y > limiteAbajo) y = limiteAbajo;
}

void Avion::acelerar()
{
    if (velocidadBase < VEL_BASE * 1.5f) {
        velocidadBase += 10.0f;
    }
}

void Avion::frenar()
{
    if (velocidadBase > VEL_BASE * 0.5f) {
        velocidadBase -= 10.0f;
    }
}

void Avion::evasion()
{
    if (!enEvasion) {
        enEvasion = true;
        tiempoEvasion = 0.0f;

        // Movimiento rápido aleatorio
        float direccion = (rand() % 2 == 0) ? -1.0f : 1.0f;
        y += direccion * 100.0f;

        if (y < limiteArriba) y = limiteArriba;
        if (y > limiteAbajo) y = limiteAbajo;

        qDebug() << "¡Evasión!";
    }
}

void Avion::activarTurbo()
{
    if (tieneTurboDisponible() && !turboActivo) {
        turboActivo = true;
        tiempoTurbo = 0.0f;
        velocidadBase = VEL_TURBO;

        qDebug() << "¡Turbo activado!";
    }
}

void Avion::aplicarOscilacion(float dt)
{
    tiempoOscilacion += dt;

    // y(t) = A·sin(ω·t)
    float oscilacion = amplitudOscilacion * std::sin(frecuenciaOscilacion * tiempoOscilacion);

    // Aplicar oscilación solo si no está en movimiento manual
    if (velocidadY == 0) {
        y += oscilacion * dt * 10.0f;  // Factor de escala
    }

    // Limitar
    if (y < limiteArriba) y = limiteArriba;
    if (y > limiteAbajo) y = limiteAbajo;
}

float Avion::getVelocidadActual() const
{
    return velocidadBase;
}

void Avion::actualizarSprite()
{
    // Determinar sprite según velocidad vertical
    if (velocidadY < -10.0f) {
        spriteActual = 1;  // Inclinado hacia arriba
    } else if (velocidadY > 10.0f) {
        spriteActual = 2;  // Inclinado hacia abajo
    } else {
        spriteActual = 0;  // Horizontal
    }

    // Resetear velocidad vertical (para la siguiente actualización)
    velocidadY = 0;
}
