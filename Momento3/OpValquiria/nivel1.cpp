#include "decoracion.h"
#include "cuadro.h"
#include "nivel1.h"
#include <QPainter>
#include <QDebug>
#include "jugador.h"
#include "guardia.h"
#include "objetointeractivo.h"
#include "planos.h"
#include "llave.h"
#include <QKeyEvent>

enum TipoTile
{
    VACIO = 0,
    PISO = 1,
    PARED = 2,
    COLUMNA = 3,
    SOMBRA = 4,
};

Nivel1::Nivel1()
    : Nivel(1),
    camaraX(0),
    jugador(nullptr),
    planos1(nullptr),
    planos2(nullptr),
    llave(nullptr),
    tienePlano1(false),
    tienePlano2(false),
    tieneLlave(false),
    puertaAbierta(false),
    detecciones(0)
{
    posicionPuerta = QPointF(41 * 64, 1 * 64);
    posicionSalida = QPointF(41 * 64, 2 * 64);
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

    for (ObjetoInteractivo* objeto : objetos)
    {
        delete objeto;
    }
    objetos.clear();

    for (Cuadro* cuadro : cuadros)
    {
        delete cuadro;
    }
    cuadros.clear();

    for (Decoracion* deco : decoraciones)
    {
        delete deco;
    }
    decoraciones.clear();
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
    crearObjetos();
    crearDecoraciones();
    detecciones = 0;
    qDebug() << "Jugador creado!";
    qDebug() << "Guardias creados:" << guardias.size();

    tienePlano1 = false;
    tienePlano2 = false;
    tieneLlave = false;
    puertaAbierta = false;
    completado = false;
}
void Nivel1::crearObjetos()
{
    qDebug() << "=== CREANDO OBJETOS ===";

    // PLANO 1: En las oficinas (habitación 2)
    planos1 = new Planos(35 * 64, 4 * 64);
    objetos.append(planos1);

    // PLANO 2: En la sala de reuniones (habitación 3)
    planos2 = new Planos(44 * 64, 3 * 64);
    objetos.append(planos2);

    // LLAVE: En el pasillo central (escondida entre cajas)
    llave = new Llave(20 * 64, 4 * 64, "puerta_salida");
    objetos.append(llave);

    qDebug() << "Objetos creados:" << objetos.size();
}
void Nivel1::crearDecoraciones()
{
    qDebug() << "=== CREANDO DECORACIONES INDEPENDIENTES ===";

    // ============================================
    // HABITACIÓN 1: ENTRADA
    // ============================================

    // Escritorios en las esquinas
    decoraciones.append(new Decoracion(1*64, 2*64 - 30, 60, 50,
                                       TipoDecoracion::ESCRITORIO, true));
    decoraciones.append(new Decoracion(10*64 + 15, 2*64 - 30, 60, 50,
                                       TipoDecoracion::ESCRITORIO, true));

    // Sillas junto a escritorios
    decoraciones.append(new Decoracion(2*64, 2*64 - 28, 40, 40,
                                       TipoDecoracion::SILLA, true));
    decoraciones.append(new Decoracion(9*64 + 25, 2*64 - 25, 40, 40,
                                       TipoDecoracion::SILLA, true));
    decoraciones.append(new Decoracion(6*64+6, 1*64, 50, 55,
                                       TipoDecoracion::CUADRO_DECORATIVO, false));
    // Plantas decorativas (NO sólidas)
    decoraciones.append(new Decoracion(1*64, 3*64, 40, 40,
                                       TipoDecoracion::PLANTA, false));
    decoraciones.append(new Decoracion(1*64, 6*64, 40, 40,
                                       TipoDecoracion::PLANTA, false));
    decoraciones.append(new Decoracion(11*64 + 20, 2*64 - 25, 40, 40,
                                       TipoDecoracion::PLANTA, false));
    decoraciones.append(new Decoracion(11*64 + 20, 6*64, 30, 50,
                                       TipoDecoracion::LAMPARA_PIE, true));

    // ============================================
    // PASILLO CENTRAL
    // ============================================

    // Archivador en el pasillo
    decoraciones.append(new Decoracion(27*64-6, 2*64 - 50, 70, 70,
                                       TipoDecoracion::ESTANTERIA, true));

    // Macetas decorativas en el pasillo
    decoraciones.append(new Decoracion(14*64 - 29, 2*64 - 16, 35, 35,
                                       TipoDecoracion::MACETA, false));
    decoraciones.append(new Decoracion(13*64, 2*64 - 16, 35, 35,
                                       TipoDecoracion::MACETA, false));
    decoraciones.append(new Decoracion(20*64, 1*64, 50, 55,
                                       TipoDecoracion::CUADRO_DECORATIVO, false));
    decoraciones.append(new Decoracion(20*64 - 40, 4*64, 120, 70,
                                       TipoDecoracion::MESA, true));

    // ============================================
    // HABITACIÓN 2: OFICINAS
    // ============================================

    // Escritorios en las oficinas
    decoraciones.append(new Decoracion(30*64, 3*64, 60, 50,
                                       TipoDecoracion::ESCRITORIO, true));
    decoraciones.append(new Decoracion(34*64, 3*64, 60, 50,
                                       TipoDecoracion::ESCRITORIO, true));
    decoraciones.append(new Decoracion(38*64, 3*64, 60, 50,
                                       TipoDecoracion::ESCRITORIO, true));

    // Sillas
    decoraciones.append(new Decoracion(30*64, 4*64 + 10, 40, 40,
                                       TipoDecoracion::SILLA, true));
    decoraciones.append(new Decoracion(34*64, 4*64 + 10, 40, 40,
                                       TipoDecoracion::SILLA, true));
    decoraciones.append(new Decoracion(38*64, 4*64 + 10, 40, 40,
                                       TipoDecoracion::SILLA, true));

    // Estantería en la pared
    decoraciones.append(new Decoracion(39*64, 2*64 + 10, 50, 55,
                                       TipoDecoracion::ESTANTERIA, true));

    // Archivador
    decoraciones.append(new Decoracion(29*64, 5*64, 45, 50,
                                       TipoDecoracion::ARCHIVADOR, true));

    // Planta decorativa
    decoraciones.append(new Decoracion(28*64 + 10, 2*64 + 20, 40, 40,
                                       TipoDecoracion::PLANTA, false));

    // ============================================
    // HABITACIÓN 3: SALA DE REUNIONES
    // ============================================

    // Mesa de reuniones grande (centro)


    // Sillas alrededor de la mesa
    decoraciones.append(new Decoracion(42*64, 3*64 + 20, 40, 40,
                                       TipoDecoracion::SILLA, true));
    decoraciones.append(new Decoracion(42*64, 4*64 + 10, 40, 40,
                                       TipoDecoracion::SILLA, true));
    decoraciones.append(new Decoracion(47*64, 3*64 + 20, 40, 40,
                                       TipoDecoracion::SILLA, true));
    decoraciones.append(new Decoracion(47*64, 4*64 + 10, 40, 40,
                                       TipoDecoracion::SILLA, true));

    // Estantería
    decoraciones.append(new Decoracion(48*64, 5*64, 50, 55,
                                       TipoDecoracion::ESTANTERIA, true));

    // Planta en esquina
    decoraciones.append(new Decoracion(41*64, 2*64 + 20, 40, 40,
                                       TipoDecoracion::PLANTA, false));

    // Cuadros decorativos en las paredes (NO interactivos)
    decoraciones.append(new Decoracion(41*64, 1*64 + 20, 40, 35,
                                       TipoDecoracion::CUADRO_DECORATIVO, false));


    qDebug() << "Total decoraciones creadas:" << decoraciones.size();
}
void Nivel1::verificarInteracciones()
{
    if (!jugador || jugador->estaMuerto()) return;

    for (ObjetoInteractivo* objeto : objetos)
    {
        if (!objeto->estaRecogido() && objeto->jugadorCerca(jugador))
        {
            // El jugador está cerca del objeto
            objeto->interactuar(jugador);

            // Actualizar estado del nivel
            if (objeto->getTipo() == TipoObjeto::PLANOS)
            {
                if (objeto == planos1)
                {
                    tienePlano1 = true;
                    qDebug() << "¡Plano 1 obtenido!";
                } else if (objeto == planos2)
                {
                    tienePlano2 = true;
                    qDebug() << "¡Plano 2 obtenido!";
                }
            } else if (objeto->getTipo() == TipoObjeto::LLAVE)
            {
                tieneLlave = true;
                qDebug() << "¡Llave obtenida!";
            }
        }
    }

    // Verificar si puede abrir la puerta
    if (tienePlano1 && tienePlano2 && tieneLlave && !puertaAbierta) {
        // El jugador está cerca de la puerta
        float distPuerta = std::sqrt(
            std::pow(jugador->getX() - posicionPuerta.x(), 2) +
            std::pow(jugador->getY() - posicionPuerta.y(), 2)
            );

        if (distPuerta < 50.0f)
        {
            abrirPuerta();
        }
    }
}
void Nivel1::abrirPuerta()
{
    puertaAbierta = true;

    // Cambiar el tile de la puerta en el mapa
    int tileX = (int)posicionPuerta.x() / TAMANO_TILE;
    int tileY = (int)posicionPuerta.y() / TAMANO_TILE;

    mapa[tileY][tileX] = PISO;  // Abrir la puerta
    mapa[tileY + 1][tileX] = PISO;

    qDebug() << "¡PUERTA ABIERTA! Dirígete a la salida";
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

    tileSombra = QPixmap(":/sprites/sprites/sombra.png");
    qDebug() << "Sombra:" << !tileSombra.isNull();

    tilePuertaCerrada = QPixmap(":/sprites/sprites/puerta1.png");
    qDebug() << "Puerta:" << !tilePuertaCerrada.isNull();

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
    //mapa[1][41]= PUERTA;

    // ==========================================
    // HABITACIÓN 1: ENTRADA (columnas 0-12)
    // ==========================================
    for (int y = 1; y < 12; y++)
    {
        mapa[y][0] = COLUMNA;   // COLUMNA 1
        if (y != 3 && y != 4 && y != 5)
        {
            mapa[y][12] = COLUMNA;  // COLUMNA 2
        }
    }
    for (int x = 1; x <= 11; x++)
    {
        for (int y = 8; y <= 10; y++)
        {
            mapa[y][x]=SOMBRA;
        }
    }

    mapa[3][12] = PARED;

    // ==========================================
    // HABITACIÓN 2: (columnas 12-28)
    // ==========================================

    // Zona de sombra en el pasillo
    for (int x = 13; x < 27; x++)
    {
        for (int y = 8; y <= 10; y++)
        {
            mapa[y][x]=SOMBRA;
        }
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
    for (int x = 33; x <= 39; x++)
    {
        for (int y = 8; y <= 10; y++)
        {
            mapa[y][x]=SOMBRA;
        }
    }


    // ==========================================
    // HABITACIÓN 4: (columnas 40-50)
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
    for(int i = 8; i <= 10; i++)
    {
        mapa[i][41] =SOMBRA;
    }
}


void Nivel1::actualizar(float dt)
{
    if (jugador)
    {
        if (jugador->estaMuerto())
        {
            jugador->actualizar(dt);
            return;
        }
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

        bool enSombra = estaEnSombra(jugador->getX(), jugador->getY());
        bool agachado = jugador->estaAgachado();

        if (enSombra && agachado)
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
    verificarInteracciones();
    //verificarCaptura();
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

    for (Decoracion* deco : decoraciones)
    {
        deco->renderizar(painter);
    }
    for (Cuadro* cuadro : cuadros)
    {
        cuadro->renderizar(painter);
    }
    for (ObjetoInteractivo* objeto : objetos)
    {
        objeto->renderizar(painter);
    }

    for (Guardia* guardia : guardias)
    {
        guardia->renderizar(painter);
    }

    if (jugador)
    {
        jugador->renderizar(painter);
    }
    if (puertaAbierta)
    {
        painter->setPen(Qt::green);
        painter->setFont(QFont("Arial", 16, QFont::Bold));
        painter->drawText(posicionPuerta.x() - 30, posicionPuerta.y() - 10, "→ SALIDA");
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
    if (jugador && !jugador->estaMuerto())
    {
        // Detecciones
        QColor colorDeteccion = (detecciones > 0) ? Qt::red : Qt::green;
        painter->setPen(colorDeteccion);
        painter->setFont(QFont("Arial", 12, QFont::Bold));
        painter->drawText(200, 20, QString("Detecciones: %1/%2")
                                      .arg(detecciones)
                                      .arg(MAX_DETECCIONES));
    }
}

void Nivel1::manejarTecla(QKeyEvent* event, bool pressed)
{
    if (jugador)
    {
        if (pressed)
        {
            if (event->key() == Qt::Key_E)
            {
                verificarInteracciones();
            }
            else
            {
                jugador->manejarTeclaPresionada(event->key());
            }
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
                if (!tileSombra.isNull())
                {
                    painter->drawPixmap(posX, posY, TAMANO_TILE, TAMANO_TILE, tileSombra);
                }
                else
                {
                    painter->fillRect(posX, posY, TAMANO_TILE, TAMANO_TILE, "black");
                }
                break;

            //case PUERTA:
                //break;
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
    if (!jugador || jugador->estaMuerto()) return false;

    // Victoria: Tiene todos los objetos Y está en la zona de salida
    if (tienePlano1 && tienePlano2 && tieneLlave && puertaAbierta)
    {
        float distSalida = std::sqrt(
            std::pow(jugador->getX() - posicionSalida.x(), 2) +
            std::pow(jugador->getY() - posicionSalida.y(), 2)
            );

        return distSalida < 50.0f;
    }
    return false;
}
bool Nivel1::verificarCondicionDerrota()
{
    if (jugador && jugador->estaMuerto()) return true;
    return detecciones >= MAX_DETECCIONES;
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
    for (Decoracion* deco : decoraciones)
    {
        if (deco->colisionaCon(x, y, ancho, alto))
        {
            return true;
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

    // GUARDIA 1: Patrulla en la entrada
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

    // GUARDIA 2
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

    // GUARDIA 3
    QVector<QPointF> ruta3;
    ruta3.append(QPointF(30 * 64, 6 * 64));
    ruta3.append(QPointF(31 * 64, 8 * 64));
    ruta3.append(QPointF(33 * 64, 3 * 64));
    ruta3.append(QPointF(35 * 64, 10 * 64));
    ruta3.append(QPointF(37 * 64, 2 * 64));
    ruta3.append(QPointF(39 * 64, 5 * 64));

    Guardia* guardia3 = new Guardia(39 * 64, 4 * 64, ruta3);
    guardia3->setJugadorObjetivo(jugador);
    guardias.append(guardia3);

    // GUARDIA 4
    QVector<QPointF> ruta4;
    ruta4.append(QPointF(42 * 64, 9 * 64));
    ruta4.append(QPointF(48 * 64, 10 * 64));
    ruta4.append(QPointF(48 * 64, 2 * 64));
    ruta4.append(QPointF(41 * 64, 2 * 64));

    Guardia* guardia4 = new Guardia(42 * 64, 6 * 64, ruta4);
    guardia4->setJugadorObjetivo(jugador);
    guardias.append(guardia4);

    QVector<QPointF> ruta5;
    ruta5.append(QPointF(45 * 64, 5 * 64));
    ruta5.append(QPointF(39 * 64, 5 * 64));
    ruta5.append(QPointF(45 * 64, 5 * 64));
    ruta5.append(QPointF(45 * 64, 10 * 64));

    Guardia* guardia5 = new Guardia(45 * 64, 7 * 64, ruta5);
    guardia5->setJugadorObjetivo(jugador);
    guardias.append(guardia5);

    qDebug() << "Total guardias creados:" << guardias.size();
}

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
