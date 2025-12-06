#include "guardia.h"
#include "jugador.h"
#include "nivel1.h"
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
    nivelRef(nullptr),
    direccionActual(DireccionGuardia::ABAJO),
    frameActual(0),
    tiempoAnimacion(0.05f),
    intentandoRodear(false),
    puntoRodeo(QPointF(0, 0)),
    tiempoAtascado(0.8f)
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

bool Guardia::verificarColision(float newX, float newY)
{
    if (!nivelRef) return false;

    // Verificar colisión con el nivel (usa hitbox del guardia)
    return nivelRef->hayColision(newX, newY, ancho, alto);
}
bool Guardia::hayLineaDeVista(QPointF desde, QPointF hasta)
{
    if (!nivelRef) return true;  // Sin nivel, asumir que puede ver

    // Calcular la distancia total
    float dx = hasta.x() - desde.x();
    float dy = hasta.y() - desde.y();
    float distancia = std::sqrt(dx*dx + dy*dy);

    // Número de puntos a verificar (cada 16 píxeles aproximadamente)
    int pasos = (int)(distancia / 16.0f);
    if (pasos < 2) pasos = 2;

    // Incremento por paso
    float stepX = dx / pasos;
    float stepY = dy / pasos;

    // Verificar cada punto en la línea
    for (int i = 1; i < pasos; i++) {
        float checkX = desde.x() + stepX * i;
        float checkY = desde.y() + stepY * i;

        // Si hay una pared en este punto, no hay línea de vista
        if (nivelRef->hayColision(checkX, checkY, 1, 1))
        {
            return false;  // ¡Hay una pared bloqueando!
        }
    }

    return true;  // Línea de vista despejada
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

    // Dibujar campo de visión, borrar
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
    direccionVista += 95.0f * dt;
    if (direccionVista > 360.0f)
    {
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
    if (jugadorObjetivo->estaOculto())
    {
        return false;
    }

    // Calcular distancia
    float distancia = calcularDistanciaAJugador();

    if (distancia > rangoVision)
    {
        return false;  // Muy lejos
    }
    QPointF posGuardia(x + ancho/2, y + alto/2);
    QPointF posJugador = jugadorObjetivo->obtenerPosicion();
    posJugador.setX(posJugador.x() + jugadorObjetivo->getAncho()/2);
    posJugador.setY(posJugador.y() + jugadorObjetivo->getAlto()/2);

    if (!hayLineaDeVista(posGuardia, posJugador))
    {
        return false;  // ¡HAY UNA PARED BLOQUEANDO!
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
    if (jugadorObjetivo->estaAgachado()) {
        return false;
    }

    float distancia = calcularDistanciaAJugador();

    // Si está fuera del rango auditivo, no puede escuchar
    if (distancia >= rangoAuditivo) {
        return false;
    }

    // ✓ NUEVO: Verificar que no haya paredes bloqueando el sonido
    // El sonido se atenúa con las paredes, pero puede atravesar algunas
    QPointF posGuardia(x + ancho/2, y + alto/2);
    QPointF posJugador = jugadorObjetivo->obtenerPosicion();
    posJugador.setX(posJugador.x() + jugadorObjetivo->getAncho()/2);
    posJugador.setY(posJugador.y() + jugadorObjetivo->getAlto()/2);

    // Contar cuántas paredes hay entre el guardia y el jugador
    int paredesEntreMedio = contarParedesEnLinea(posGuardia, posJugador);

    // Si hay más de 2 paredes, el sonido está muy atenuado
    if (paredesEntreMedio > 2) {
        return false;
    }

    // Si hay 1-2 paredes, reducir el rango efectivo
    float rangoEfectivo = rangoAuditivo;
    if (paredesEntreMedio > 0) {
        rangoEfectivo = rangoAuditivo * 0.5f; // 50% del rango con paredes
    }

    return distancia < rangoEfectivo;
}
int Guardia::contarParedesEnLinea(QPointF desde, QPointF hasta)
{
    if (!nivelRef) return 0;

    float dx = hasta.x() - desde.x();
    float dy = hasta.y() - desde.y();
    float distancia = std::sqrt(dx*dx + dy*dy);

    int pasos = (int)(distancia / 16.0f);
    if (pasos < 2) pasos = 2;

    float stepX = dx / pasos;
    float stepY = dy / pasos;

    int contadorParedes = 0;
    bool enPared = false;

    for (int i = 1; i < pasos; i++)
    {
        float checkX = desde.x() + stepX * i;
        float checkY = desde.y() + stepY * i;

        bool hayPared = nivelRef->hayColision(checkX, checkY, 1, 1);

        // Contar solo cuando entra a una nueva pared (evitar contar la misma pared múltiples veces)
        if (hayPared && !enPared)
        {
            contadorParedes++;
            enPared = true;
        }
        else if (!hayPared)
        {
            enPared = false;
        }
    }

    return contadorParedes;
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

    if (distancia > 1.0f) {
        // Normalizar
        float dxNorm = dx / distancia;
        float dyNorm = dy / distancia;

        // Actualizar dirección de vista
        actualizarDireccionVista(dxNorm, dyNorm);
        actualizarDireccionSprite(dxNorm, dyNorm);

        float prevX = x;
        float prevY = y;

        // Calcular nueva posición
        float movX = dxNorm * velocidad * dt;
        float movY = dyNorm * velocidad * dt;

        float newX = x + movX;
        float newY = y + movY;

        //VERIFICAR COLISIÓN antes de mover
        if (!verificarColision(newX, newY))
        {
            // No hay colisión, mover normalmente
            x = newX;
            y = newY;
            velocidadX = movX / dt;
            velocidadY = movY / dt;
        }
        else
        {
            bool movioAlgo = false;

            // Intentar solo movimiento en X
            if (!verificarColision(newX, prevY))
            {
                x = newX;
                y = prevY;
                velocidadX = movX / dt;
                velocidadY = 0;
                movioAlgo = true;
            }
            // Intentar solo movimiento en Y
            else if (!verificarColision(prevX, newY))
            {
                x = prevX;
                y = newY;
                velocidadX = 0;
                velocidadY = movY / dt;
                movioAlgo = true;
            }
            if (!movioAlgo)
            {
                velocidadX = 0;
                velocidadY = 0;
                tiempoAtascado += dt;

                // Si lleva más de 1 segundo atascado, intentar rodeo
                if (tiempoAtascado > 1.0f)
                {
                    intentarRodearObstaculo(objetivo, dt);
                }

                // Si está persiguiendo y lleva mucho tiempo atascado, abandonar
                if (estadoActual == EstadoAgente::PERSIGUIENDO && tiempoAtascado > 3.0f) {
                    qDebug() << "Guardia atascado demasiado tiempo, volviendo a patrullar";
                    cambiarEstado(EstadoAgente::PATRULLANDO);
                    tiempoAtascado = 0.0f;
                }
            } else {
                tiempoAtascado = 0.0f;
            }
        }
    } else {
        velocidadX = 0;
        velocidadY = 0;
        tiempoAtascado = 0.0f;
        intentandoRodear = false;
    }
}
void Guardia::intentarRodearObstaculo(QPointF objetivo, float dt)
{
    const float TILE_SIZE = 64.0f;

    // Calcular dirección general hacia el objetivo
    float dx = objetivo.x() - x;
    float dy = objetivo.y() - y;

    // Intentar 4 direcciones alternativas en orden de prioridad
    QVector<QPointF> direccionesIntento;

    // Si el objetivo está principalmente a la derecha/izquierda
    if (std::abs(dx) > std::abs(dy)) {
        if (dy > 0) {
            // Objetivo abajo-derecha/izquierda: intentar bajar
            direccionesIntento.append(QPointF(x, y + TILE_SIZE));
            direccionesIntento.append(QPointF(x, y - TILE_SIZE));
        } else {
            // Objetivo arriba-derecha/izquierda: intentar subir
            direccionesIntento.append(QPointF(x, y - TILE_SIZE));
            direccionesIntento.append(QPointF(x, y + TILE_SIZE));
        }

        if (dx > 0) {
            direccionesIntento.append(QPointF(x + TILE_SIZE, y));
        } else {
            direccionesIntento.append(QPointF(x - TILE_SIZE, y));
        }
    }
    // Si el objetivo está principalmente arriba/abajo
    else {
        if (dx > 0) {
            // Objetivo derecha-arriba/abajo: intentar ir a la derecha
            direccionesIntento.append(QPointF(x + TILE_SIZE, y));
            direccionesIntento.append(QPointF(x - TILE_SIZE, y));
        } else {
            // Objetivo izquierda-arriba/abajo: intentar ir a la izquierda
            direccionesIntento.append(QPointF(x - TILE_SIZE, y));
            direccionesIntento.append(QPointF(x + TILE_SIZE, y));
        }

        if (dy > 0) {
            direccionesIntento.append(QPointF(x, y + TILE_SIZE));
        } else {
            direccionesIntento.append(QPointF(x, y - TILE_SIZE));
        }
    }

    // Probar cada dirección
    for (const QPointF& nuevaPosicion : direccionesIntento)
    {
        if (!verificarColision(nuevaPosicion.x(), nuevaPosicion.y()))
        {
            // Encontró un camino libre, moverse hacia allí
            float dirX = nuevaPosicion.x() - x;
            float dirY = nuevaPosicion.y() - y;
            float dist = std::sqrt(dirX*dirX + dirY*dirY);

            if (dist > 0)
            {
                float vel = VEL_PATRULLA; // Más lento al rodear
                if (estadoActual == EstadoAgente::PERSIGUIENDO)
                {
                    vel = VEL_INVESTIGAR; // Velocidad media
                }

                float movX = (dirX / dist) * vel * dt;
                float movY = (dirY / dist) * vel * dt;

                x += movX;
                y += movY;
                velocidadX = movX / dt;
                velocidadY = movY / dt;

                actualizarDireccionSprite(dirX, dirY);
                actualizarDireccionVista(dirX, dirY);

                tiempoAtascado = 0.0f;
                return;
            }
        }
    }

    // Si no encontró ninguna dirección libre, quedarse quieto
    velocidadX = 0;
    velocidadY = 0;
}
void Guardia::actualizarDireccionVista(float dx, float dy)
{
    // Solo actualizar si hay movimiento significativo
    if (std::abs(dx) > 0.1f || std::abs(dy) > 0.1f)
    {
        // Calcular ángulo del movimiento en grados
        float angulo = std::atan2(dy, dx) * 180.0f / M_PI;

        // Normalizar a rango 0-360
        if (angulo < 0)
        {
            angulo += 360.0f;
        }

        direccionVista = angulo;
    }
}
