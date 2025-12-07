#include "misil.h"
#include "avion.h"
#include <QPainter>
#include <QDebug>
#include <cmath>

Misil::Misil(float x, float y, Avion* objetivo)
    : AgenteInteligente(x, y, 30, 15),
    objetivo(objetivo),
    leadFactor(1.0f),
    fallosConsecutivos(0),
    velocidadMisil(VEL_BASE),
    estadoMisil(EstadoMisil::BUSQUEDA),
    tiempoVida(0.0f),
    tiempoMaximoVida(15.0f),
    detonado(false),
    tiempoExplosion(0.0f),
    radioExplosion(0.0f),
    rotacion(0.0f)
{
    estadoActual = EstadoAgente::PATRULLANDO;
    cargarSprite();

    qDebug() << "Misil creado en:" << x << "," << y;
}

Misil::~Misil()
{
}

void Misil::cargarSprite()
{
    spriteMisil = QPixmap(":/sprites/sprites/misil.png");

    if (spriteMisil.isNull()) {
        qDebug() << "Advertencia: No se pudo cargar sprite de misil";
    }
}

// ============================================
// CICLO DE IA
// ============================================

void Misil::percibir()
{
    if (!objetivo || !activo || detonado) return;

    // Percibir posición y velocidad del objetivo
    posicionPredicha = calcularPosicionFutura();
}

void Misil::razonar()
{
    if (!objetivo || !activo || detonado) return;

    float distancia = calcularDistanciaAObjetivo();

    // Máquina de estados
    switch (estadoMisil) {
    case EstadoMisil::BUSQUEDA:
        if (distancia < 800.0f) {
            estadoMisil = EstadoMisil::PERSECUCION;
            qDebug() << "Misil: BUSQUEDA -> PERSECUCION";
        }
        break;

    case EstadoMisil::PERSECUCION:
        if (distancia < 200.0f) {
            estadoMisil = EstadoMisil::PREDICCION;
            qDebug() << "Misil: PERSECUCION -> PREDICCION";
        }
        break;

    case EstadoMisil::PREDICCION:
        if (distancia < 50.0f) {
            estadoMisil = EstadoMisil::EXPLOSION;
            qDebug() << "Misil: PREDICCION -> EXPLOSION";
        } else if (distancia > 300.0f) {
            estadoMisil = EstadoMisil::PERSECUCION;
        }
        break;

    case EstadoMisil::EXPLOSION:
        explotar();
        break;
    }
}

void Misil::actuar(float dt)
{
    if (!objetivo || !activo || detonado) return;

    switch (estadoMisil) {
    case EstadoMisil::BUSQUEDA:
        buscar();
        break;

    case EstadoMisil::PERSECUCION:
        perseguir();
        break;

    case EstadoMisil::PREDICCION:
        predecir();
        break;

    case EstadoMisil::EXPLOSION:
        // Ya se maneja en razonar()
        break;
    }

    actualizarRotacion();
}

void Misil::aprenderDeExperiencia()
{
    fallosConsecutivos++;

    // Por cada 2 fallos, mejorar predicción
    if (fallosConsecutivos >= 2) {
        leadFactor += 0.1f;

        if (leadFactor > 2.5f) {
            leadFactor = 2.5f;  // Límite máximo
        }

        fallosConsecutivos = 0;
        qDebug() << "Misil mejoró predicción. leadFactor:" << leadFactor;
    }
}

// ============================================
// ACTUALIZACIÓN Y RENDERIZADO
// ============================================

void Misil::actualizar(float dt)
{
    if (!activo) return;

    tiempoVida += dt;

    // Eliminar si excede tiempo de vida
    if (tiempoVida >= tiempoMaximoVida) {
        activo = false;
        qDebug() << "Misil autodestruido por tiempo";
        return;
    }

    if (detonado) {
        tiempoExplosion += dt;
        radioExplosion += 200.0f * dt;

        if (tiempoExplosion >= 0.5f) {
            activo = false;
        }
        return;
    }

    // Ejecutar ciclo de IA
    percibir();
    razonar();
    actuar(dt);
}

void Misil::renderizar(QPainter* painter)
{
    if (!activo || !painter) return;

    if (detonado) {
        // Dibujar explosión
        painter->save();

        int alpha = 255 - (int)(tiempoExplosion * 510.0f);
        if (alpha < 0) alpha = 0;

        painter->setBrush(QColor(255, 100, 0, alpha));
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(x - radioExplosion/2, y - radioExplosion/2,
                             radioExplosion, radioExplosion);

        painter->setBrush(QColor(255, 255, 0, alpha / 2));
        painter->drawEllipse(x - radioExplosion/4, y - radioExplosion/4,
                             radioExplosion/2, radioExplosion/2);

        painter->restore();
        return;
    }

    painter->save();

    // Rotar sprite
    painter->translate(x + ancho/2, y + alto/2);
    painter->rotate(rotacion);
    painter->translate(-ancho/2, -alto/2);

    if (!spriteMisil.isNull()) {
        painter->drawPixmap(0, 0, ancho, alto, spriteMisil);
    } else {
        // Fallback: Misil rojo con punta
        painter->setBrush(QColor(200, 50, 50));
        painter->setPen(QPen(Qt::black, 2));

        QPolygonF misil;
        misil << QPointF(0, alto/2)           // Punta izquierda
              << QPointF(ancho - 5, 0)        // Aleta superior
              << QPointF(ancho, alto/2)       // Punta derecha
              << QPointF(ancho - 5, alto)     // Aleta inferior
              << QPointF(0, alto/2);

        painter->drawPolygon(misil);

        // Llama trasera
        painter->setBrush(QColor(255, 150, 0));
        painter->drawEllipse(-8, alto/2 - 3, 8, 6);
    }

    painter->restore();

// Debug: trayectoria predicha
#ifdef DEBUG_IA
    painter->setPen(QPen(Qt::red, 1, Qt::DashLine));
    painter->drawLine(x, y, posicionPredicha.x(), posicionPredicha.y());
#endif
}

// ============================================
// COMPORTAMIENTOS
// ============================================

void Misil::buscar()
{
    if (!objetivo) return;

    // Moverse directamente hacia la posición actual del objetivo
    QPointF posObjetivo(objetivo->getX(), objetivo->getY());
    QPointF direccion = calcularDireccionAObjetivo(posObjetivo);

    velocidadX = direccion.x() * velocidadMisil;
    velocidadY = direccion.y() * velocidadMisil;

    x += velocidadX * 0.016f;
    y += velocidadY * 0.016f;
}

void Misil::perseguir()
{
    if (!objetivo) return;

    // Moverse hacia la posición actual con velocidad incrementada
    QPointF posObjetivo(objetivo->getX(), objetivo->getY());
    QPointF direccion = calcularDireccionAObjetivo(posObjetivo);

    float velActual = VEL_BASE * 1.2f;

    velocidadX = direccion.x() * velActual;
    velocidadY = direccion.y() * velActual;

    x += velocidadX * 0.016f;
    y += velocidadY * 0.016f;
}

void Misil::predecir()
{
    if (!objetivo) return;

    // Moverse hacia la posición PREDICHA
    QPointF direccion = calcularDireccionAObjetivo(posicionPredicha);

    float velActual = VEL_BASE * 1.5f;

    velocidadX = direccion.x() * velActual;
    velocidadY = direccion.y() * velActual;

    x += velocidadX * 0.016f;
    y += velocidadY * 0.016f;
}

void Misil::explotar()
{
    if (!detonado) {
        detonado = true;
        tiempoExplosion = 0.0f;
        radioExplosion = 10.0f;
        qDebug() << "¡EXPLOSIÓN!";
    }
}

// ============================================
// CÁLCULOS
// ============================================

QPointF Misil::calcularPosicionFutura()
{
    if (!objetivo) return QPointF(x, y);

    // Calcular tiempo de intercepción estimado
    float distancia = calcularDistanciaAObjetivo();
    float leadTime = distancia / velocidadMisil;

    // Obtener velocidad del objetivo (aproximada)
    float velObjetivoX = objetivo->getVelocidadActual();
    float velObjetivoY = 0.0f;  // Simplificación: asumimos movimiento principalmente horizontal

    // Predecir posición futura
    float predX = objetivo->getX() + velObjetivoX * leadTime * leadFactor;
    float predY = objetivo->getY() + velObjetivoY * leadTime * leadFactor;

    return QPointF(predX, predY);
}

void Misil::actualizarRotacion()
{
    if (velocidadX == 0 && velocidadY == 0) return;

    // Calcular ángulo en grados
    rotacion = std::atan2(velocidadY, velocidadX) * 180.0f / M_PI;
}

float Misil::calcularDistanciaAObjetivo() const
{
    if (!objetivo) return 9999.0f;

    float dx = objetivo->getX() - x;
    float dy = objetivo->getY() - y;

    return std::sqrt(dx*dx + dy*dy);
}

QPointF Misil::calcularDireccionAObjetivo(QPointF puntoObjetivo)
{
    float dx = puntoObjetivo.x() - x;
    float dy = puntoObjetivo.y() - y;

    float magnitud = std::sqrt(dx*dx + dy*dy);

    if (magnitud < 0.01f) {
        return QPointF(1.0f, 0.0f);
    }

    // Normalizar
    return QPointF(dx / magnitud, dy / magnitud);
}
