#include "guardia.h"
#include "jugador.h"
#include <QPainter>
#include <QPainterPath>
#include <QDebug>
#include <cmath>

Guardia::Guardia(float x, float y, const QVector<QPointF>& ruta)
    : AgenteInteligente(x, y, 32, 48),
    rutaPatrullaje(ruta),
    puntoActual(0),
    avanzandoEnRuta(true),
    campoVision(80.0f),        // campo visual
    rangoVision(150.0f),       // píxeles de alcance visual
    rangoAuditivo(180.0f),     // píxeles de alcance auditivo
    direccionVista(0.0f),
    jugadorVisto(false),
    tiempoBusqueda(0.0f),
    tiempoMaximoBusqueda(5.0f),
    jugadorObjetivo(nullptr),
    direccionActual(DireccionGuardia::ABAJO),
    frameActual(0),
    tiempoAnimacion(0.0f)
{
    estadoActual = EstadoAgente::PATRULLANDO;
    sprites.resize(4);
    cargarSprites();
}

Guardia::~Guardia()
{
}

void Guardia::cargarSprites()
{
    qDebug() << "=== CARGANDO SPRITES DEL GUARDIA ===";

    // DIRECCIÓN ARRIBA (índice 0) - 3 frames
    qDebug() << "Cargando sprites ARRIBA:";
    sprites[static_cast<int>(DireccionGuardia::ARRIBA)].clear();
    sprites[static_cast<int>(DireccionGuardia::ARRIBA)].append(
        QPixmap(":/sprites/sprites/GuardiaU1.png"));
    sprites[static_cast<int>(DireccionGuardia::ARRIBA)].append(
        QPixmap(":/sprites/sprites/GuardiaU2.png"));
    sprites[static_cast<int>(DireccionGuardia::ARRIBA)].append(
        QPixmap(":/sprites/sprites/GuardiaU3.png"));

    // DIRECCIÓN ABAJO (índice 1) - 3 frames
    qDebug() << "Cargando sprites ABAJO:";
    sprites[static_cast<int>(DireccionGuardia::ABAJO)].clear();
    sprites[static_cast<int>(DireccionGuardia::ABAJO)].append(
        QPixmap(":/sprites/sprites/GuardiaD1.png"));
    sprites[static_cast<int>(DireccionGuardia::ABAJO)].append(
        QPixmap(":/sprites/sprites/GuardiaD2.png"));
    sprites[static_cast<int>(DireccionGuardia::ABAJO)].append(
        QPixmap(":/sprites/sprites/GuardiaD3.png"));

    // DIRECCIÓN IZQUIERDA (índice 2) - 2 frames
    qDebug() << "Cargando sprites IZQUIERDA:";
    sprites[static_cast<int>(DireccionGuardia::IZQUIERDA)].clear();
    sprites[static_cast<int>(DireccionGuardia::IZQUIERDA)].append(
        QPixmap(":/sprites/sprites/GuardiaL1.png"));
    sprites[static_cast<int>(DireccionGuardia::IZQUIERDA)].append(
        QPixmap(":/sprites/sprites/GuardiaL2.png"));

    // DIRECCIÓN DERECHA (índice 3) - 2 frames
    // Opción 2: Si tienes sprites separados para derecha, usa esto:
    qDebug() << "Cargando sprites DERECHA:";
    sprites[static_cast<int>(DireccionGuardia::DERECHA)].clear();
    sprites[static_cast<int>(DireccionGuardia::DERECHA)].append(
        QPixmap(":/sprites/sprites/GuardiaR1.png"));
    sprites[static_cast<int>(DireccionGuardia::DERECHA)].append(
        QPixmap(":/sprites/sprites/GuardiaR2.png"));

    // Verificar carga
    for (int dir = 0; dir < 4; dir++)
    {
        qDebug() << "Dirección" << dir << ":" << sprites[dir].size() << "frames";
        for (int frame = 0; frame < sprites[dir].size(); frame++)
        {
            qDebug() << "  Frame" << frame << ":" << !sprites[dir][frame].isNull();
        }
    }

    qDebug() << "=== FIN CARGA SPRITES GUARDIA ===";
}

int Guardia::getNumFrames(DireccionGuardia dir) const
{
    int dirIndex = static_cast<int>(dir);
    if (dirIndex >= 0 && dirIndex < sprites.size())
    {
        return sprites[dirIndex].size();
    }
    return 0;
}

void Guardia::actualizarDireccionSprite(float dx, float dy)
{
    // Determinar dirección principal del movimiento
    if (std::abs(dx) > std::abs(dy)) {
        // Movimiento horizontal
        if (dx > 0) {
            direccionActual = DireccionGuardia::DERECHA;
        } else {
            direccionActual = DireccionGuardia::IZQUIERDA;
        }
    } else {
        // Movimiento vertical
        if (dy > 0) {
            direccionActual = DireccionGuardia::ABAJO;
        } else {
            direccionActual = DireccionGuardia::ARRIBA;
        }
    }
}

// ============================================
// CICLO DE IA
// ============================================

void Guardia::percibir()
{
    if (!jugadorObjetivo || !activo) return;

    // Resetear percepción
    jugadorVisto = false;

    // 1. PERCEPCIÓN VISUAL
    if (detectarJugador()) {
        jugadorVisto = true;
        ultimaPosicionJugador = jugadorObjetivo->obtenerPosicion();
    }

    // 2. PERCEPCIÓN AUDITIVA
    if (detectarRuido()) {
        // Escuchó algo, investigar
        if (estadoActual == EstadoAgente::PATRULLANDO) {
            ultimaPosicionJugador = jugadorObjetivo->obtenerPosicion();
        }
    }
}

void Guardia::razonar()
{
    if (!activo) return;

    // Máquina de estados finitos (FSM)
    switch (estadoActual) {

    case EstadoAgente::PATRULLANDO:
        if (jugadorVisto) {
            cambiarEstado(EstadoAgente::PERSIGUIENDO);
        } else if (detectarRuido()) {
            cambiarEstado(EstadoAgente::INVESTIGANDO);
        }
        break;

    case EstadoAgente::INVESTIGANDO:
        if (jugadorVisto) {
            cambiarEstado(EstadoAgente::PERSIGUIENDO);
        } else {
            // Si llegó al punto de investigación
            float distancia = std::sqrt(
                std::pow(x - ultimaPosicionJugador.x(), 2) +
                std::pow(y - ultimaPosicionJugador.y(), 2)
                );

            if (distancia < 30.0f) {
                cambiarEstado(EstadoAgente::BUSCANDO);
                tiempoBusqueda = 0.0f;
            }
        }
        break;

    case EstadoAgente::PERSIGUIENDO:
        if (!jugadorVisto) {
            cambiarEstado(EstadoAgente::BUSCANDO);
            tiempoBusqueda = 0.0f;
        }
        // Si está muy cerca del jugador = GAME OVER (lo implementarás después)
        break;

    case EstadoAgente::BUSCANDO:
        if (jugadorVisto) {
            cambiarEstado(EstadoAgente::PERSIGUIENDO);
        } else if (tiempoBusqueda > tiempoMaximoBusqueda) {
            // Volver a patrullar
            cambiarEstado(EstadoAgente::PATRULLANDO);
            aprenderDeExperiencia();
        }
        break;
    }
}

void Guardia::actuar(float dt)
{
    if (!activo) return;

    // Ejecutar comportamiento según estado
    switch (estadoActual) {
    case EstadoAgente::PATRULLANDO:
        patrullar(dt);
        rotarVision(dt);
        break;

    case EstadoAgente::INVESTIGANDO:
        investigar(dt, ultimaPosicionJugador);
        break;

    case EstadoAgente::PERSIGUIENDO:
        perseguir(dt);
        break;

    case EstadoAgente::BUSCANDO:
        buscar(dt);
        break;
    }
}

void Guardia::aprenderDeExperiencia()
{
    // Registrar zona donde detectó al jugador
    zonasDeteccion.append(ultimaPosicionJugador);

    // Incrementar factor de aprendizaje
    factorAprendizaje += 0.1f;

    qDebug() << "Guardia aprendió de la experiencia. Factor:" << factorAprendizaje;
    qDebug() << "Zonas de detección registradas:" << zonasDeteccion.size();
}

// ============================================
// ACTUALIZACIÓN Y RENDERIZADO
// ============================================

void Guardia::actualizar(float dt)
{
    if (!activo) return;

    // 1. Percibir entorno
    percibir();

    // 2. Razonar
    razonar();

    // 3. Actuar
    actuar(dt);

    // 4. Actualizar animación
    bool estaMoviendose = (velocidadX != 0 || velocidadY != 0);

    if (estaMoviendose)
    {
        tiempoAnimacion += dt;

        float velocidadAnimacion = 0.15f;
        if (estadoActual == EstadoAgente::PERSIGUIENDO)
        {
            velocidadAnimacion = 0.08f;  // Más rápido
        }

        if (tiempoAnimacion > velocidadAnimacion)
        {
            tiempoAnimacion = 0.0f;

            int numFrames = getNumFrames(direccionActual);
            if (numFrames > 0)
            {
                frameActual = (frameActual + 1) % numFrames;
            }
        }
    } else {
        // Si no se mueve, usar frame idle (0)
        frameActual = 0;
        tiempoAnimacion = 0.0f;
    }
}

void Guardia::renderizar(QPainter* painter)
{
    if (!activo || !painter) return;

    // Obtener sprite actual
    int dirIndex = static_cast<int>(direccionActual);
    QPixmap spriteActual;

    if (dirIndex >= 0 && dirIndex < sprites.size()) {
        if (frameActual >= 0 && frameActual < sprites[dirIndex].size()) {
            spriteActual = sprites[dirIndex][frameActual];
        }
    }

    // Dibujar sprite o fallback
    if (!spriteActual.isNull()) {
        // ⭐ DIBUJAR EL SPRITE
        painter->drawPixmap(x, y, ancho, alto, spriteActual);

        // Agregar tinte según estado (opcional)
        if (estadoActual == EstadoAgente::PERSIGUIENDO) {
            painter->fillRect(x, y, ancho, alto, QColor(255, 0, 0, 50));
        } else if (estadoActual == EstadoAgente::INVESTIGANDO) {
            painter->fillRect(x, y, ancho, alto, QColor(255, 255, 0, 30));
        }
    } else {
        // FALLBACK: Rectángulo de color
        QColor color;
        switch (estadoActual) {
        case EstadoAgente::PATRULLANDO:
            color = QColor(100, 100, 200);
            break;
        case EstadoAgente::INVESTIGANDO:
            color = QColor(200, 200, 100);
            break;
        case EstadoAgente::PERSIGUIENDO:
            color = QColor(200, 100, 100);
            break;
        case EstadoAgente::BUSCANDO:
            color = QColor(200, 150, 100);
            break;
        }

        painter->fillRect(x, y, ancho, alto, color);
        painter->setPen(QPen(Qt::black, 2));
        painter->drawRect(x, y, ancho, alto);
    }

    // Dibujar campo de visión
    painter->save();
    painter->translate(x + ancho/2, y + alto/2);
    painter->rotate(direccionVista);

    QPainterPath camino;
    camino.moveTo(0, 0);
    camino.arcTo(-rangoVision, -rangoVision,
                 rangoVision*2, rangoVision*2,
                 -campoVision/2, campoVision);
    camino.lineTo(0, 0);

    painter->setBrush(QColor(255, 255, 0, 30));
    painter->setPen(QPen(QColor(255, 255, 0, 100), 1));
    painter->drawPath(camino);

    painter->restore();

    // Indicador de estado
    painter->setPen(Qt::white);
    painter->setFont(QFont("Arial", 8));


    // Debug: mostrar frame actual
    painter->setPen(Qt::yellow);
    painter->drawText(x, y - 15, QString("F:%1 D:%2")
                                     .arg(frameActual)
                                     .arg(static_cast<int>(direccionActual)));
}

// ============================================
// COMPORTAMIENTOS
// ============================================

void Guardia::patrullar(float dt)
{
    if (rutaPatrullaje.isEmpty()) return;

    QPointF objetivo = rutaPatrullaje[puntoActual];
    moverHaciaPunto(objetivo, VEL_PATRULLA, dt);

    // Verificar si llegó al punto
    float distancia = std::sqrt(
        std::pow(x - objetivo.x(), 2) +
        std::pow(y - objetivo.y(), 2)
        );

    if (distancia < 10.0f) {
        // Avanzar al siguiente punto
        if (avanzandoEnRuta) {
            puntoActual++;
            if (puntoActual >= rutaPatrullaje.size()) {
                puntoActual = rutaPatrullaje.size() - 2;
                avanzandoEnRuta = false;
            }
        } else {
            puntoActual--;
            if (puntoActual < 0) {
                puntoActual = 1;
                avanzandoEnRuta = true;
            }
        }
    }
}

void Guardia::investigar(float dt, QPointF objetivo)
{
    moverHaciaPunto(objetivo, VEL_INVESTIGAR, dt);
}

void Guardia::perseguir(float dt)
{
    if (!jugadorObjetivo) return;

    QPointF posJugador = jugadorObjetivo->obtenerPosicion();
    moverHaciaPunto(posJugador, VEL_PERSEGUIR, dt);

    // Actualizar dirección de vista hacia el jugador
    direccionVista = calcularAnguloAJugador();
}

void Guardia::buscar(float dt)
{
    tiempoBusqueda += dt;

    // Rotar lentamente mirando alrededor
    direccionVista += 30.0f * dt;
    if (direccionVista > 360.0f) {
        direccionVista -= 360.0f;
    }
}

// ============================================
// DETECCIÓN
// ============================================

bool Guardia::detectarJugador()
{
    if (!jugadorObjetivo) return false;

    // El jugador está oculto = no se puede ver
    if (jugadorObjetivo->estaOculto()) {
        return false;
    }

    // Calcular distancia
    float distancia = calcularDistanciaAJugador();

    if (distancia > rangoVision) {
        return false;  // Muy lejos
    }

    // Calcular ángulo al jugador
    float anguloAJugador = calcularAnguloAJugador();

    // Verificar si está en el campo de visión
    return estaEnCampoVision(anguloAJugador);
}

bool Guardia::detectarRuido()
{
    if (!jugadorObjetivo) return false;

    // Solo detecta ruido si el jugador está corriendo
    if (jugadorObjetivo->estaAgachado() ) {
        return false;
    }

    float distancia = calcularDistanciaAJugador();
    return distancia < rangoAuditivo;
}

void Guardia::rotarVision(float dt)
{
    // Rotación lenta mientras patrulla
    direccionVista += 15.0f * dt;
    if (direccionVista > 360.0f) {
        direccionVista -= 360.0f;
    }
}

// ============================================
// MÉTODOS AUXILIARES
// ============================================

float Guardia::calcularDistanciaAJugador() const
{
    if (!jugadorObjetivo) return 999999.0f;

    QPointF posJugador = jugadorObjetivo->obtenerPosicion();
    return std::sqrt(
        std::pow(x - posJugador.x(), 2) +
        std::pow(y - posJugador.y(), 2)
        );
}

float Guardia::calcularAnguloAJugador() const
{
    if (!jugadorObjetivo) return 0.0f;

    QPointF posJugador = jugadorObjetivo->obtenerPosicion();
    float dx = posJugador.x() - x;
    float dy = posJugador.y() - y;

    float angulo = std::atan2(dy, dx) * 180.0f / M_PI;
    if (angulo < 0) angulo += 360.0f;

    return angulo;
}

bool Guardia::estaEnCampoVision(float anguloAJugador) const
{
    // Normalizar ángulos
    float diferencia = std::abs(direccionVista - anguloAJugador);

    if (diferencia > 180.0f)
    {
        diferencia = 360.0f - diferencia;
    }

    return diferencia <= (campoVision / 2.0f);
}

void Guardia::moverHaciaPunto(QPointF objetivo, float velocidad, float dt)
{
    float dx = objetivo.x() - x;
    float dy = objetivo.y() - y;

    float distancia = std::sqrt(dx*dx + dy*dy);

    if (distancia > 1.0f)
    {
        dx = dx / distancia;
        dy = dy / distancia;
        actualizarDireccionVista(dx, dy);
        actualizarDireccionSprite(dx, dy);

        dx *= velocidad * dt;
        dy *= velocidad * dt;

        x += dx;
        y += dy;
    }
}
void Guardia::actualizarDireccionVista(float dx, float dy)
{
    // Solo actualizar si hay movimiento significativo
    if (std::abs(dx) > 0.1f || std::abs(dy) > 0.1f) {
        // Calcular ángulo del movimiento en grados
        float angulo = std::atan2(dy, dx) * 180.0f / M_PI;

        // Normalizar a rango 0-360
        if (angulo < 0) {
            angulo += 360.0f;
        }

        direccionVista = angulo;
    }
}
