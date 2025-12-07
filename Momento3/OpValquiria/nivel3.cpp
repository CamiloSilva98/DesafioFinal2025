#include "nivel3.h"
#include "jugador.h"
#include "avion.h"
#include "misil.h"
#include "obstaculo.h"
#include <QPainter>
#include <QDebug>
#include <QKeyEvent>
#include <cmath>

Nivel3::Nivel3()
    : Nivel(3),
    faseActual(Fase::TERRESTRE),
    tiempoFase(0.0f),
    tiempoLimite(40.0f),
    jugador(nullptr),
    avion(nullptr),
    scrollX(0.0f),
    velocidadBase(100.0f),
    enSalto(false),
    velocidadSaltoY(0.0f),
    tiempoSalto(0.0f),
    tiempoGeneracionMisil(0.0f),
    misilesEsquivados(0),
    offsetFondo1(0.0f),
    offsetFondo2(0.0f),
    choques(0),
    llegandoAvion(false)
{
}

Nivel3::~Nivel3()
{
    if (jugador) delete jugador;
    if (avion) delete avion;

    qDeleteAll(obstaculosTerrestre);
    qDeleteAll(obstaculosAereos);
    qDeleteAll(misiles);
}

void Nivel3::inicializar()
{
    qDebug() << "Inicializando Nivel 3...";

    // Limpiar si se reinicia
    if (jugador) { delete jugador; jugador = nullptr; }
    if (avion)   { delete avion;   avion   = nullptr; }

    qDeleteAll(obstaculosTerrestre);
    obstaculosTerrestre.clear();

    qDeleteAll(obstaculosAereos);
    obstaculosAereos.clear();

    qDeleteAll(misiles);
    misiles.clear();

    // FASE 1: Escape terrestre
    jugador = new Jugador(300, 400);  // Un poco más centrado
    generarObstaculosTerrestre();

    faseActual = Fase::TERRESTRE;
    tiempoFase = 0.0f;
    scrollX = 0.0f;
    completado = false;
    choques = 0;
    enSalto = false;
    velocidadSaltoY = 0.0f;

    qDebug() << "Nivel 3 inicializado - FASE TERRESTRE";
}

void Nivel3::actualizar(float dt)
{
    tiempoFase += dt;

    if (faseActual == Fase::TERRESTRE) {
        actualizarFaseTerrestre(dt);
    } else if (faseActual == Fase::AEREO) {
        actualizarFaseAerea(dt);
    }
}

void Nivel3::actualizarFaseTerrestre(float dt)
{
    if (!jugador) return;

    // Actualizar animación / estado interno del jugador (como en Nivel 1)
    jugador->actualizar(dt);

    // Movimiento automático del jugador hacia la derecha
    float velocidad = velocidadBase;
    if (jugador->estaCorriendo()) {
        velocidad = 150.0f;   // si tu Jugador pone estaCorriendo() al presionar algo
    }

    // Avanzar al jugador en el mundo
    float nuevaX = jugador->getX() + velocidad * dt;
    jugador->setX(nuevaX);

    // La cámara sigue al jugador
    scrollX = jugador->getX();

    // Gravedad y salto
    if (enSalto) {
        aplicarGravedad(dt);
    }

    // Actualizar obstáculos (para debug o animaciones; si no quieres que se muevan solos,
    // pon velocidadScroll = 0 en su constructor)
    for (Obstaculo* obs : obstaculosTerrestre) {
        obs->actualizar(dt);
    }

    // Verificar colisiones
    verificarColisionesTerrestre();

    // Verificar si llegó al final (punto donde estaría el avión)
    if (scrollX >= ANCHO_NIVEL - 500) {
        qDebug() << "Fin de la fase terrestre (llegaría al avión aquí).";
        // Más adelante aquí llamaremos a cambiarFase() para el avión
        completado = true; // Por ahora puedes marcarlo como completado
    }
}

void Nivel3::actualizarFaseAerea(float dt)
{
    if (!avion) return;

    // Actualizar avión
    avion->actualizar(dt);
    avion->aplicarOscilacion(dt);

    // Generar misiles periódicamente
    tiempoGeneracionMisil += dt;
    if (tiempoGeneracionMisil >= 3.0f) {
        generarMisil();
        tiempoGeneracionMisil = 0.0f;
    }

    // Actualizar misiles
    for (int i = misiles.size() - 1; i >= 0; --i) {
        misiles[i]->actualizar(dt);

        if (misiles[i]->debeEliminarse()) {
            delete misiles[i];
            misiles.remove(i);
            misilesEsquivados++;
        }
    }

    // Actualizar obstáculos aéreos
    for (Obstaculo* obs : obstaculosAereos) {
        obs->actualizar(dt);
    }

    // Actualizar parallax scrolling
    actualizarParallax(dt);

    // Verificar colisiones
    verificarColisionesAereas();

    // Verificar si llegó a zona segura
    if (tiempoFase >= 90.0f) {
        completado = true;
        qDebug() << "¡Nivel 3 completado!";
    }
}

void Nivel3::renderizar(QPainter* painter)
{
    if (!painter) return;

    if (faseActual == Fase::TERRESTRE) {
        renderizarFaseTerrestre(painter);
    } else if (faseActual == Fase::AEREO) {
        renderizarFaseAerea(painter);
    }
}

void Nivel3::renderizarFaseTerrestre(QPainter* painter)
{
    // Fondo
    painter->fillRect(0, 0, 1280, 720, QColor(135, 206, 250)); // Cielo
    painter->fillRect(0, 500, 1280, 220, QColor(100, 100, 100)); // Pista

    painter->save();

    // Cámara: centramos al jugador alrededor de x = 300 en pantalla
    painter->translate(-scrollX + 300, 0);

    // Obstáculos
    for (Obstaculo* obs : obstaculosTerrestre) {
        obs->renderizar(painter);
    }

    // Jugador
    if (jugador) {
        jugador->renderizar(painter);
    }

    painter->restore();

    // HUD
    painter->setPen(Qt::white);
    painter->setFont(QFont("Arial", 12, QFont::Bold));
    painter->drawText(10, 30, QString("FASE 1: ESCAPE TERRESTRE"));
    painter->drawText(10, 50, QString("Distancia: %1 / %2")
                                  .arg((int)scrollX)
                                  .arg(ANCHO_NIVEL));
    painter->drawText(10, 70, QString("Choques: %1 / %2")
                                  .arg(choques)
                                  .arg(MAX_CHOQUES));
}

void Nivel3::renderizarFaseAerea(QPainter* painter)
{
    // Fondo con parallax
    painter->fillRect(0, 0, 1280, 720, QColor(135, 206, 235));

    // Nubes (capa de fondo)
    painter->fillRect((int)offsetFondo1, 100, 200, 80, QColor(255, 255, 255, 100));
    painter->fillRect((int)offsetFondo2, 200, 150, 60, QColor(255, 255, 255, 100));

    // Obstáculos
    for (Obstaculo* obs : obstaculosAereos) {
        obs->renderizar(painter);
    }

    // Misiles
    for (Misil* misil : misiles) {
        misil->renderizar(painter);
    }

    // Avión
    if (avion) {
        avion->renderizar(painter);
    }

    // HUD
    painter->setPen(Qt::white);
    painter->setFont(QFont("Arial", 12, QFont::Bold));
    painter->drawText(10, 30, QString("FASE 2: ESCAPE AÉREO"));
    painter->drawText(10, 50, QString("Tiempo: %1s").arg((int)tiempoFase));
    painter->drawText(10, 70, QString("Misiles esquivados: %1").arg(misilesEsquivados));

    if (avion && avion->tieneTurboDisponible()) {
        painter->setPen(Qt::green);
        painter->drawText(10, 90, "[SHIFT] Turbo listo");
    }
}

void Nivel3::cambiarFase()
{
    faseActual = Fase::AEREO;
    tiempoFase = 0.0f;

    // Crear avión
    avion = new Avion(400, 360);

    // Generar obstáculos aéreos
    generarObstaculosAereos();

    qDebug() << "Cambiando a FASE AÉREA";
}

void Nivel3::generarObstaculosTerrestre()
{
    // Generar vallas y barreras distribuidas a lo largo del nivel
    for (int i = 500; i < ANCHO_NIVEL; i += 300) {
        if (i % 600 == 0) {
            obstaculosTerrestre.append(new Obstaculo(i, 450, TipoObstaculo::VALLA, 0.0f));
        } else {
            obstaculosTerrestre.append(new Obstaculo(i, 480, TipoObstaculo::BARRERA_BAJA, 0.0f));
        }
    }

    qDebug() << "Obstáculos terrestres generados:" << obstaculosTerrestre.size();
}

void Nivel3::generarObstaculosAereos()
{
    // Generar torres y globos
    for (int i = 0; i < 10; i++) {
        float x = 1500 + i * 600;
        float y = 100 + (i % 3) * 200;

        if (i % 2 == 0) {
            obstaculosAereos.append(new Obstaculo(x, y, TipoObstaculo::TORRE));
        } else {
            obstaculosAereos.append(new Obstaculo(x, y, TipoObstaculo::GLOBO));
        }
    }

    qDebug() << "Obstáculos aéreos generados:" << obstaculosAereos.size();
}

void Nivel3::generarMisil()
{
    if (!avion) return;

    // Generar misil fuera de pantalla a la derecha
    float x = 1300;
    float y = 100 + (rand() % 400);

    Misil* nuevoMisil = new Misil(x, y, avion);
    misiles.append(nuevoMisil);

    qDebug() << "Misil generado en:" << x << y;
}

void Nivel3::aplicarGravedad(float dt)
{
    velocidadSaltoY += GRAVEDAD * dt;

    float nuevaY = jugador->getY() + velocidadSaltoY * dt;
    jugador->setY(nuevaY);

    // Aterrizar
    if (jugador->getY() >= 400) {
        jugador->setY(400);
        enSalto = false;
        velocidadSaltoY = 0.0f;
    }
}

void Nivel3::manejarSalto()
{
    if (!enSalto) {
        enSalto = true;
        velocidadSaltoY = -400.0f;  // Velocidad inicial hacia arriba
        qDebug() << "¡Salto!";
    }
}

void Nivel3::verificarColisionesTerrestre()
{
    if (!jugador) return;

    // Rectángulo del jugador (usamos su bounding box completa por ahora)
    QRectF rectJugador(jugador->getX(), jugador->getY(),
                       jugador->getAncho(), jugador->getAlto());

    for (Obstaculo* obs : obstaculosTerrestre)
    {
        if (!obs || !obs->estaActivo() || !obs->esSolido()) continue;

        QRectF rectObs(obs->getX(), obs->getY(),
                       obs->getAncho(), obs->getAlto());

        if (rectJugador.intersects(rectObs))
        {
            // Dependiendo del tipo, debería reaccionar distinto,
            // pero por ahora cualquier golpe cuenta como choque.
            choques++;
            qDebug() << "Choque terrestre. Total choques:" << choques;

            // Pequeño retroceso del jugador para que no se quede pegado
            jugador->setX(jugador->getX() - 50);
        }
    }
}

void Nivel3::verificarColisionesAereas()
{
    if (!avion) return;

    QRectF rectAvion(avion->getX(), avion->getY(),
                     avion->getAncho(), avion->getAlto());

    // Colisiones con misiles
    for (Misil* misil : misiles)
    {
        if (!misil || !misil->estaActivo()) continue;

        QRectF rectMisil(misil->getX(), misil->getY(),
                         misil->getAncho(), misil->getAlto());

        if (rectAvion.intersects(rectMisil))
        {
            qDebug() << "¡Impacto de misil en el avión!";
            choques++;
            misil->setActivo(false);
        }
    }

    // Colisiones con obstáculos aéreos sólidos
    for (Obstaculo* obs : obstaculosAereos)
    {
        if (!obs || !obs->estaActivo() || !obs->esSolido()) continue;

        QRectF rectObs(obs->getX(), obs->getY(),
                       obs->getAncho(), obs->getAlto());

        if (rectAvion.intersects(rectObs))
        {
            qDebug() << "Colisión con obstáculo aéreo";
            choques++;
            // Puedes desactivar el obstáculo para no chocar mil veces
            // obs->setActivo(false);
        }
    }
}

void Nivel3::actualizarParallax(float dt)
{
    offsetFondo1 -= 30.0f * dt;
    offsetFondo2 -= 50.0f * dt;

    if (offsetFondo1 < -1280) offsetFondo1 = 0;
    if (offsetFondo2 < -1280) offsetFondo2 = 0;
}

bool Nivel3::verificarCondicionVictoria()
{
    return completado;
}

bool Nivel3::verificarCondicionDerrota()
{
    if (faseActual == Fase::TERRESTRE)
    {
        return choques >= MAX_CHOQUES;
    } else {
        return false;  // TODO: Implementar derrota en fase aérea
    }
}

void Nivel3::manejarTecla(QKeyEvent* event, bool pressed)
{
    if (faseActual == Fase::TERRESTRE)
    {
        if (pressed) {
            if (event->key() == Qt::Key_Space)
            {
                manejarSalto();
            }
            else if (jugador) {
                jugador->manejarTeclaPresionada(event->key());
            }
        } else {
            if (jugador) {
                jugador->manejarTeclaSoltada(event->key());
            }
        }
    }
    else if (faseActual == Fase::AEREO && avion)
    {
        // Por ahora, movimiento discreto cuando se pulsa la tecla.
        // Si luego quieres movimiento suave, lo hacemos.
        float dtControl = 0.016f; // ~1 frame

        if (pressed)
        {
            if (event->key() == Qt::Key_Up || event->key() == Qt::Key_W) {
                avion->moverArriba(dtControl);
            } else if (event->key() == Qt::Key_Down || event->key() == Qt::Key_S) {
                avion->moverAbajo(dtControl);
            } else if (event->key() == Qt::Key_Shift) {
                avion->activarTurbo();
            }
            // Si quieres que acelerar/frenar funcionen:
            // else if (event->key() == Qt::Key_Right || event->key() == Qt::Key_D) { avion->acelerar(); }
            // else if (event->key() == Qt::Key_Left || event->key() == Qt::Key_A) { avion->frenar(); }
        }
        // No hacemos nada en key release para la fase aérea de momento
    }
}
