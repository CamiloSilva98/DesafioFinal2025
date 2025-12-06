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

    // FASE 1: Escape terrestre
    jugador = new Jugador(100, 400);
    generarObstaculosTerrestre();

    faseActual = Fase::TERRESTRE;
    tiempoFase = 0.0f;
    completado = false;

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

    // Actualizar jugador
    jugador->actualizar(dt);

    // Movimiento automático hacia la derecha
    float velocidad = velocidadBase;
    if (jugador->estaCorriendo()) {
        velocidad = 150.0f;
    }

    scrollX += velocidad * dt;

    // Gravedad y salto
    if (enSalto) {
        aplicarGravedad(dt);
    }

    // Actualizar obstáculos
    for (Obstaculo* obs : obstaculosTerrestre) {
        obs->actualizar(dt);
    }

    // Verificar colisiones
    verificarColisionesTerrestre();

    // Verificar si llegó al final (avión)
    if (scrollX >= ANCHO_NIVEL - 500) {
        qDebug() << "¡Llegó al avión! Cambiando a FASE AÉREA";
        cambiarFase();
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
    painter->translate(-scrollX + 100, 0);

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
    // Generar vallas y barreras distribuidas
    for (int i = 500; i < ANCHO_NIVEL; i += 300) {
        if (i % 600 == 0) {
            obstaculosTerrestre.append(new Obstaculo(i, 450, TipoObstaculo::VALLA));
        } else {
            obstaculosTerrestre.append(new Obstaculo(i, 480, TipoObstaculo::BARRERA_BAJA));
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
    // TODO: Implementar colisiones con obstáculos
}

void Nivel3::verificarColisionesAereas()
{
    if (!avion) return;

    // Colisiones con misiles
    for (Misil* misil : misiles) {
        if (avion->colisionaCon(misil)) {
            qDebug() << "¡Impacto de misil!";
            // TODO: Implementar game over
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
    if (pressed) {
        if (faseActual == Fase::TERRESTRE)
        {
            if (event->key() == Qt::Key_Space)
            {
                manejarSalto();
            } else if (jugador) {
                jugador->manejarTeclaPresionada(event->key());
            }
        } else if (faseActual == Fase::AEREO && avion)
        {
            // Controles del avión
            // TODO: Implementar controles
        }
    } else {
        if (jugador) {
            jugador->manejarTeclaSoltada(event->key());
        }
    }
}
