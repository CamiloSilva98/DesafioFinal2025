#include "nivel1.h"
#include <QPainter>
#include <QDebug>
#include "jugador.h"
#include "guardia.h"

enum TipoTile
{
    VACIO = 0,
    PISO = 1,
    PARED = 2,
    COLUMNA = 3,
    SOMBRA = 4,
    PUERTA = 5,
    VENTANA = 6
};

Nivel1::Nivel1()
    : Nivel(1), camaraX(0), jugador(nullptr)
{
}

Nivel1::~Nivel1()
{
    if (jugador)
    {
        delete jugador;
    }
    for (Guardia* guardia : guardias)
    {
        delete guardia;
    }
    guardias.clear();
}

void Nivel1::inicializar()
{
    qDebug() << "Inicializando Nivel 1...";

    cargarSprites();
    crearMapa();
    qDebug() << "Mapa creado!";

    jugador = new Jugador(80, 150);
    qDebug() << "Jugador creado!";

    crearGuardias();
    detecciones = 0;
    qDebug() << "Jugador creado!";
    qDebug() << "Guardias creados:" << guardias.size();

    completado = false;
}
void Nivel1::cargarSprites()
{
    qDebug() << "=== CARGANDO SPRITES ===";

    tilePiso = QPixmap(":/sprites/sprites/Piso.png");
    qDebug() << "Piso:" << !tilePiso.isNull() << "Size:" << tilePiso.width() << "x" << tilePiso.height();

    tilePared = QPixmap(":/sprites/sprites/wall.png");
    qDebug() << "Pared:" << !tilePared.isNull();

    tileColumna = QPixmap(":/sprites/sprites/Columna.png");
    qDebug() << "Columna:" << !tileColumna.isNull();

    tileVentana = QPixmap(":/sprites/sprites/Piso.png");
    qDebug() << "Ventana:" << !tileVentana.isNull();

    tileLampara = QPixmap(":/sprites/sprites/Piso.png");
    qDebug() << "Lampara:" << !tileLampara.isNull();

    tileSombra = QPixmap(":/sprites/sprites/Piso.png");
    qDebug() << "Sombra:" << !tileSombra.isNull();

    qDebug() << "=== FIN CARGA SPRITES ===";
}
void Nivel1::crearMapa()
{
    // Inicializar todo como VACIO
    for (int y = 0; y < ALTO_MAPA; y++)
    {
        for (int x = 0; x < ANCHO_MAPA; x++)
        {
            mapa[y][x] = VACIO;
        }
    }

    // ==========================================
    // CREAR EL DISEÑO DEL CUARTEL
    // ==========================================

    // Piso general del cuartel (filas 8-11 = suelo)
    for (int y = 2; y < 12; y++)
    {
        for (int x = 0; x < ANCHO_MAPA; x++)
        {
            mapa[y][x] = PISO;
        }
    }


    // Techo/pared superior (fila 7)
    for (int x = 1; x < ANCHO_MAPA-1; x++)
    {
        if(x != 41)
        {
            mapa[1][x] = PARED;
        }
    }
    for (int x = 1; x < ANCHO_MAPA-1; x++)
    {
        mapa[11][x] = COLUMNA;
    }
    mapa[1][41]= PUERTA;

    // ==========================================
    // HABITACIÓN 1: ENTRADA (columnas 0-12)
    // ==========================================
    for (int y = 1; y < 12; y++)
    {
        mapa[y][0] = COLUMNA;   // COLUMNA 1
        if (y != 3 && y != 4 && y != 5)
        {  // Dejar espacio para puerta
            mapa[y][12] = COLUMNA;  // COLUMNA 2
        }
    }

    mapa[3][12] = PARED;

    // ==========================================
    // HABITACIÓN 2: (columnas 12-28)
    // ==========================================

    // Zona de sombra en el pasillo
    for (int x = 18; x < 22; x++)
    {
        mapa[4][x] = SOMBRA;
        mapa[5][x] = SOMBRA;
    }

    // ==========================================
    // HABITACIÓN 3: (columnas 28-40)
    // ==========================================
    for (int y = 1; y < 12; y++)
    {
        if (y != 8 && y != 9 && y != 10)
        {
            mapa[y][28] = COLUMNA;  // Pared izquierda
        }
    }
    mapa[8][28]=PARED;
    for (int x = 35; x < 38; x++)
    {
        mapa[4][x] = SOMBRA;
    }


    // ==========================================
    // HABITACIÓN 3: (columnas 40-50)
    // ==========================================
    for (int y = 1; y < 12; y++)
    {
        if (y != 3 && y != 4 && y != 5)
        {
            mapa[y][40] = COLUMNA;
        }
        mapa[y][49] = COLUMNA;
    }

    for (int x = 40; x < 46; x++)
    {
        mapa[3][x] = COLUMNA;
        mapa[4][x] = PARED;
    }
    mapa[6][41] = COLUMNA;
    mapa[7][41] = PARED;
    for(int y = 3; y < 7; y++)
    {
        mapa[y][46] = COLUMNA;
    }
    mapa[7][46] = PARED;
}

void Nivel1::actualizar(float dt)
{
    if (jugador)
    {
        // Guardar posición anterior
        float prevX = jugador->getX();
        float prevY = jugador->getY();

        // Actualizar jugador (calcula nueva posición)
        jugador->actualizar(dt);

        // Obtener nueva posición
        float newX = jugador->getX();
        float newY = jugador->getY();

        // Verificar colisión en nueva posición
        bool colisionTotal = hayColision(newX, newY,
                                         jugador->getAncho(),
                                         jugador->getAlto());

        if (colisionTotal)
        {
            // Sistema de "sliding" - permitir movimiento en un eje si el otro está bloqueado

            // Intentar movimiento solo en X (mantener Y anterior)
            bool colisionX = hayColision(newX, prevY,
                                         jugador->getAncho(),
                                         jugador->getAlto());

            // Intentar movimiento solo en Y (mantener X anterior)
            bool colisionY = hayColision(prevX, newY,
                                         jugador->getAncho(),
                                         jugador->getAlto());

            if (!colisionX && colisionY)
            {
                // Permitir solo movimiento horizontal
                jugador->setY(prevY);
            }
            else if (colisionX && !colisionY)
            {
                // Permitir solo movimiento vertical
                jugador->setX(prevX);
            }
            else
            {
                // Bloquear ambos movimientos
                jugador->setX(prevX);
                jugador->setY(prevY);
            }
        }

        // Verificar si está en zona de sombra Y agachado
        if (estaEnSombra(jugador->getX(), jugador->getY()) &&
            jugador->estaAgachado())
        {
            jugador->setOculto(true);
        }
        else
        {
            jugador->setOculto(false);
        }
    }
    for (Guardia* guardia : guardias)
    {
        guardia->actualizar(dt);
    }
    verificarDetecciones();
    actualizarCamara();
}
void Nivel1::actualizarCamara()
{
    if (!jugador) return;

    const float ANCHO_PANTALLA = 1550;
    const float ANCHO_NIVEL = ANCHO_MAPA * TAMANO_TILE;

    // Centrar cámara en el jugador
    camaraX = jugador->getX() - ANCHO_PANTALLA / 2;

    // Limitar la cámara
    if (camaraX < 0) camaraX = 0;
    if (camaraX > ANCHO_NIVEL - ANCHO_PANTALLA)
    {
        camaraX = ANCHO_NIVEL - ANCHO_PANTALLA;
    }
}
void Nivel1::renderizar(QPainter* painter)
{
    if (!painter) return;

    // Guardar estado del painter
    painter->save();

    // Aplicar scroll de cámara
    painter->translate(-camaraX, 0);

    // Dibujar el mapa
    dibujarMapa(painter);
    for (Guardia* guardia : guardias)
    {
        guardia->renderizar(painter);
    }

    if (jugador)
    {
        jugador->renderizar(painter);
    }

    // Restaurar painter
    painter->restore();

    // ========================================
    // HUD - Información en pantalla
    // ========================================

    painter->setPen(Qt::white);
    painter->setFont(QFont("Arial", 10, QFont::Bold));
    painter->drawText(10, 15, "Nivel 1: La Guarida del Lobo");

    painter->setFont(QFont("Arial", 9));
    painter->drawText(10, 45, "WASD - Mover | Shift - Correr | C - Agacharse");
}

void Nivel1::manejarTecla(QKeyEvent* event, bool pressed)
{
    if (jugador)
    {
        if (pressed)
        {
            jugador->manejarTeclaPresionada(event->key());
        }
        else
        {
            jugador->manejarTeclaSoltada(event->key());
        }
    }
}
void Nivel1::dibujarMapa(QPainter* painter)
{
    for (int y = 0; y < ALTO_MAPA; y++)
    {
        for (int x = 0; x < ANCHO_MAPA; x++)
        {
            int posX = x * TAMANO_TILE;
            int posY = y * TAMANO_TILE;

            switch (mapa[y][x])
            {
            case VACIO:
                // Cielo/exterior - azul oscuro nocturno
                painter->fillRect(posX, posY, TAMANO_TILE, TAMANO_TILE,
                                 "black");
                break;

            case PISO:
                if (!tilePiso.isNull())
                {
                    painter->drawPixmap(posX, posY, TAMANO_TILE, TAMANO_TILE, tilePiso);
                }
                else
                {
                    painter->fillRect(posX, posY, TAMANO_TILE, TAMANO_TILE, Qt::red);
                }
                break;

            case PARED:
                if (!tilePared.isNull())
                {
                    painter->drawPixmap(posX, posY, TAMANO_TILE, TAMANO_TILE, tilePared);
                }
                else
                {
                    painter->fillRect(posX, posY, TAMANO_TILE, TAMANO_TILE, QColor(95, 100, 110));
                }
                break;

            case COLUMNA:
                painter->drawPixmap(posX, posY, TAMANO_TILE, TAMANO_TILE, tileColumna);
                break;

            case SOMBRA:
                // Piso normal
                painter->fillRect(posX, posY, TAMANO_TILE, TAMANO_TILE,
                                  QColor(55, 60, 70));

                // Capa de sombra semi-transparente
                painter->fillRect(posX, posY, TAMANO_TILE, TAMANO_TILE,
                                  QColor(0, 0, 0, 140));
                break;

            case PUERTA:
                // Piso
                painter->fillRect(posX, posY, TAMANO_TILE, TAMANO_TILE,
                                  QColor(55, 60, 70));

                // Marco de la puerta
                painter->fillRect(posX + 8, posY, 48, TAMANO_TILE,
                                  QColor(60, 45, 30));

                // Puerta en sí (más clara)
                painter->fillRect(posX + 12, posY, 40, TAMANO_TILE,
                                  QColor(90, 65, 40));

                // Líneas verticales de la puerta
                painter->setPen(QColor(70, 50, 35));
                painter->drawLine(posX + 22, posY, posX + 22, posY + TAMANO_TILE);
                painter->drawLine(posX + 32, posY, posX + 32, posY + TAMANO_TILE);
                painter->drawLine(posX + 42, posY, posX + 42, posY + TAMANO_TILE);

                // Manija dorada
                painter->fillRect(posX + 44, posY + TAMANO_TILE/2 - 4,
                                  8, 8, QColor(200, 180, 50));
                break;

            case VENTANA:
                // Pared
                painter->fillRect(posX, posY, TAMANO_TILE, TAMANO_TILE,
                                  QColor(95, 100, 110));

                // Ventana (marco)
                painter->fillRect(posX + 12, posY + 16, 40, 40,
                                  QColor(40, 45, 55));

                // Cristal azulado (reflejo de la noche)
                painter->fillRect(posX + 16, posY + 20, 32, 32,
                                  QColor(60, 80, 120, 180));

                // Cruz de la ventana
                painter->setPen(QPen(QColor(80, 80, 90), 3));
                painter->drawLine(posX + 32, posY + 20, posX + 32, posY + 52);
                painter->drawLine(posX + 16, posY + 36, posX + 48, posY + 36);
                break;
            }
        }
    }
}

void Nivel1::moverCamara(float dx)
{
    camaraX += dx;

    // Limitar la cámara para que no se salga del mapa
    const float ANCHO_PANTALLA = 800;
    const float ANCHO_NIVEL = ANCHO_MAPA * TAMANO_TILE;

    if (camaraX < 0) camaraX = 0;
    if (camaraX > ANCHO_NIVEL - ANCHO_PANTALLA)
    {
        camaraX = ANCHO_NIVEL - ANCHO_PANTALLA;
    }
}

bool Nivel1::verificarCondicionVictoria()
{
    return false;  // Por ahora no hay condición de victoria
}

bool Nivel1::verificarCondicionDerrota()
{
    return false;  // Por ahora no hay condición de derrota
}

bool Nivel1::hayColision(float x, float y, float ancho, float alto) const
{
    // Calcular las esquinas del rectángulo del jugador
    int izquierda = static_cast<int>(x) / TAMANO_TILE;
    int derecha = static_cast<int>(x + ancho - 1) / TAMANO_TILE;
    int arriba = static_cast<int>(y) / TAMANO_TILE;
    int abajo = static_cast<int>(y + alto - 1) / TAMANO_TILE;

    // Verificar límites del mapa
    if (izquierda < 0 || derecha >= ANCHO_MAPA ||
        arriba < 0 || abajo >= ALTO_MAPA)
    {
        return true;  // Fuera del mapa = colisión
    }

    // Verificar cada tile que ocupa el jugador
    for (int ty = arriba; ty <= abajo; ty++)
    {
        for (int tx = izquierda; tx <= derecha; tx++)
        {
            int tile = mapa[ty][tx];

            // Colisión con PARED o CAJA
            if (tile == PARED || tile == COLUMNA)
            {
                return true;
            }
        }
    }

    return false;  // No hay colisión
}

int Nivel1::getTileEnPosicion(float x, float y) const
{
    int tileX = static_cast<int>(x) / TAMANO_TILE;
    int tileY = static_cast<int>(y) / TAMANO_TILE;

    // Verificar límites
    if (tileX < 0 || tileX >= ANCHO_MAPA ||
        tileY < 0 || tileY >= ALTO_MAPA)
    {
        return VACIO;
    }

    return mapa[tileY][tileX];
}

bool Nivel1::estaEnSombra(float x, float y) const
{
    // Verificar el centro del jugador
    int centerX = static_cast<int>(x + 16) / TAMANO_TILE;
    int centerY = static_cast<int>(y + 24) / TAMANO_TILE;

    if (centerX < 0 || centerX >= ANCHO_MAPA ||
        centerY < 0 || centerY >= ALTO_MAPA)
    {
        return false;
    }

    return mapa[centerY][centerX] == SOMBRA;
}
void Nivel1::crearGuardias()
{
    qDebug() << "=== CREANDO GUARDIAS ===";

    // GUARDIA 1: Patrulla en la entrada (habitación 1)
    QVector<QPointF> ruta1;
    ruta1.append(QPointF(11 * 64, 9 * 64));
    ruta1.append(QPointF(8 * 64, 3 * 64));
    ruta1.append(QPointF(1 * 64, 2 * 64));
    ruta1.append(QPointF(7 * 64, 5 * 64));
    ruta1.append(QPointF(1 * 64, 6 * 64));
    ruta1.append(QPointF(1 * 64, 10 * 64));
    ruta1.append(QPointF(4 * 64, 7 * 64));
    ruta1.append(QPointF(6 * 64, 9 * 64));
    ruta1.append(QPointF(9 * 64, 6 * 64));
    ruta1.append(QPointF(9 * 64, 3 * 64));

    Guardia* guardia1 = new Guardia(11 * 64, 2 * 64, ruta1);
    guardia1->setJugadorObjetivo(jugador);
    guardias.append(guardia1);

    // GUARDIA 2: Patrulla en el pasillo central
    QVector<QPointF> ruta2;
    ruta2.append(QPointF(13 * 64, 7 * 64));
    ruta2.append(QPointF(16 * 64, 3 * 64));
    ruta2.append(QPointF(18 * 64, 10 * 64));
    ruta2.append(QPointF(20 * 64, 4 * 64));
    ruta2.append(QPointF(23 * 64, 8 * 64));
    ruta2.append(QPointF(25 * 64, 3 * 64));
    ruta2.append(QPointF(27 * 64, 8 * 64));

    Guardia* guardia2 = new Guardia(27 * 64, 10 * 64, ruta2);
    guardia2->setJugadorObjetivo(jugador);
    guardias.append(guardia2);

    // GUARDIA 3: Patrulla en las oficinas (habitación 2)
    QVector<QPointF> ruta3;
    ruta3.append(QPointF(30 * 64, 6 * 64));
    ruta3.append(QPointF(31 * 64, 8 * 64));
    ruta3.append(QPointF(33 * 64, 3 * 64));
    ruta3.append(QPointF(35 * 64, 10 * 64));
    ruta3.append(QPointF(37 * 64, 2 * 64));
    ruta3.append(QPointF(39 * 64, 5 * 64));
    ruta3.append(QPointF(42 * 64, 5 * 64));

    Guardia* guardia3 = new Guardia(45 * 64, 5 * 64, ruta3);
    guardia3->setJugadorObjetivo(jugador);
    guardias.append(guardia3);

    // GUARDIA 4: Patrulla cerca del objetivo (sala de reuniones)
    QVector<QPointF> ruta4;
    ruta4.append(QPointF(42 * 64, 9 * 64));
    ruta4.append(QPointF(48 * 64, 10 * 64));
    ruta4.append(QPointF(48 * 64, 2 * 64));
    ruta4.append(QPointF(41 * 64, 2 * 64));

    Guardia* guardia4 = new Guardia(42 * 64, 6 * 64, ruta4);
    guardia4->setJugadorObjetivo(jugador);
    guardias.append(guardia4);

    qDebug() << "Total guardias creados:" << guardias.size();
}

// ============================================
// NUEVO MÉTODO: verificarDetecciones()
// ============================================
void Nivel1::verificarDetecciones()
{
    for (Guardia* guardia : guardias)
    {
        if (!guardia->estaActivo()) continue;

        // Si un guardia detectó al jugador
        if (guardia->haDetectadoJugador())
        {
            // Solo contar si acabó de detectarlo (evitar múltiples conteos)
            if (guardia->getEstado() == EstadoAgente::PERSIGUIENDO)
            {
                // Verificar si es una nueva detección
                static int ultimoFrameDeteccion = -100;
                static int frameActual = 0;
                frameActual++;

                if (frameActual - ultimoFrameDeteccion > 60)
                {  // 1 segundo
                    detecciones++;
                    ultimoFrameDeteccion = frameActual;
                    qDebug() << "¡DETECTADO! Total detecciones:" << detecciones;
                }
            }
        }
    }
}
