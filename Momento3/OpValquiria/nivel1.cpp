#include "nivel1.h"
#include <QPainter>
#include <QDebug>

//revisar tu ruta de sprites
const QString RUTA_SPRITES = "C:/Users/AsusTUF/Desktop/U/2025-2/Info II/DesafioFinal-JuegoWWII/DesafioFinal2025";

enum TipoTile
{
    VACIO = 0,
    PISO = 1,
    PARED = 2,
    CAJA = 3,
    SOMBRA = 4,
    PUERTA = 5,
    VENTANA = 6,
    LAMPARA = 7
};

Nivel1::Nivel1()
    : Nivel(1), camaraX(0)
{
}

Nivel1::~Nivel1()
{
}

void Nivel1::inicializar()
{
    qDebug() << "Inicializando Nivel 1...";

    cargarSprites();
    crearMapa();
    qDebug() << "Mapa creado!";
    completado = false;
}
void Nivel1::cargarSprites()
{
    qDebug() << "=== CARGANDO SPRITES ===";

    tilePiso = QPixmap(RUTA_SPRITES + "/Momento3/sprites/Piso.png");
    qDebug() << "Piso:" << !tilePiso.isNull() << "Size:" << tilePiso.width() << "x" << tilePiso.height();

    tilePared = QPixmap(RUTA_SPRITES + "/Momento3/sprites/wall.png");
    qDebug() << "Pared:" << !tilePared.isNull();

    tileCaja = QPixmap(RUTA_SPRITES + "/Momento3/sprites/Piso.png");
    qDebug() << "Caja:" << !tileCaja.isNull();

    tileVentana = QPixmap(RUTA_SPRITES + "/Momento3/sprites/Piso.png");
    qDebug() << "Ventana:" << !tileVentana.isNull();

    tileLampara = QPixmap(RUTA_SPRITES + "/Momento3/sprites/Piso.png");
    qDebug() << "Lampara:" << !tileLampara.isNull();

    tileSombra = QPixmap(RUTA_SPRITES + "/Momento3/sprites/Piso.png");
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
    for (int y = 2; y < 6; y++)
    {
        for (int x = 0; x < ANCHO_MAPA; x++)
        {
            mapa[y][x] = PISO;
        }
    }

    // Techo/pared superior (fila 7)
    for (int x = 0; x <= ANCHO_MAPA; x++)
    {
        mapa[1][x] = PARED;
    }

    // ==========================================
    // HABITACIÓN 1: ENTRADA (columnas 0-12)
    // ==========================================
    for (int y = 2; y < 6; y++)
    {
        mapa[y][0] = PARED;   // Pared izquierda
        if (y != 3 && y != 4)
        {  // Dejar espacio para puerta
            mapa[y][12] = PARED;  // Pared derecha
        }
    }

    // Puerta de entrada
    mapa[3][12] = PUERTA;
    mapa[4][12] = PUERTA;

    // Decoración: caja en la esquina
    mapa[4][2] = CAJA;

    // ==========================================
    // PASILLO CENTRAL (columnas 12-28)
    // ==========================================
    // Solo piso, sin paredes laterales

    // Zona de sombra en el pasillo
    for (int x = 18; x < 22; x++)
    {
        mapa[4][x] = SOMBRA;
        mapa[5][x] = SOMBRA;
    }

    // Cajas en el pasillo para ocultarse
    mapa[3][15] = CAJA;
    mapa[3][25] = CAJA;

    // Lámparas del pasillo
    mapa[1][16] = LAMPARA;
    mapa[1][24] = LAMPARA;


    // ==========================================
    // HABITACIÓN 2: OFICINAS (columnas 28-40)
    // ==========================================
    for (int y = 2; y < 6; y++)
    {
        if (y != 9 && y != 10)
        {
            mapa[y][28] = PARED;  // Pared izquierda
        }
        mapa[y][40] = PARED;      // Pared derecha
    }

    // Puerta de entrada a oficinas
    mapa[3][28] = PUERTA;
    mapa[4][28] = PUERTA;

    // Ventanas en la pared superior
    mapa[1][32] = VENTANA;
    mapa[1][36] = VENTANA;

    // Mobiliario de oficina (cajas representan escritorios)
    mapa[3][30] = CAJA;
    mapa[3][34] = CAJA;
    mapa[3][38] = CAJA;

    // Zona de sombra bajo escritorio
    for (int x = 35; x < 38; x++) {
        mapa[4][x] = SOMBRA;
    }

    // Lámpara
    mapa[1][34] = LAMPARA;


    // ==========================================
    // HABITACIÓN 3: SALA DE REUNIONES - OBJETIVO (columnas 40-50)
    // ==========================================
    for (int y = 2; y < 6; y++) {
        if (y != 3 && y != 4) {
            mapa[y][40] = PARED;  // Pared izquierda
        }
        mapa[y][49] = PARED;      // Pared derecha
    }

    // Puerta de entrada
    mapa[3][40] = PUERTA;
    mapa[4][40] = PUERTA;

    // Mesa de reuniones (representada con cajas)
    for (int x = 43; x < 47; x++) {
        mapa[3][x] = CAJA;
    }

    // Ventanas
    mapa[1][44] = VENTANA;
    mapa[1][46] = VENTANA;

    // Lámpara central
    mapa[1][45] = LAMPARA;
}

void Nivel1::actualizar(float dt)
{
    // Por ahora solo dejamos que pase el tiempo
    // La cámara se moverá manualmente con las flechas
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

    // Restaurar painter
    painter->restore();

    // Dibujar información en pantalla (HUD)
    painter->setPen(Qt::white);
    painter->setFont(QFont("Arial", 12));
    painter->drawText(10, 20, "Nivel 1: La Guarida del Lobo");
    painter->drawText(10, 40, "Use ← → para mover la cámara");
    painter->drawText(10, 60, QString("Posición cámara: %1").arg(camaraX));
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

            case CAJA:
                // Primero dibujar el piso debajo
                painter->fillRect(posX, posY, TAMANO_TILE, TAMANO_TILE,
                                  QColor(55, 60, 70));

                // Caja de madera marrón
                painter->fillRect(posX + 6, posY + 6,
                                  TAMANO_TILE - 12, TAMANO_TILE - 12,
                                  QColor(101, 67, 33));

                // Borde más oscuro (sombra)
                painter->setPen(QPen(QColor(70, 45, 20), 2));
                painter->drawRect(posX + 6, posY + 6,
                                  TAMANO_TILE - 12, TAMANO_TILE - 12);

                // Detalles de la caja (líneas de madera)
                painter->setPen(QColor(85, 55, 28));
                painter->drawLine(posX + 20, posY + 6, posX + 20, posY + TAMANO_TILE - 6);
                painter->drawLine(posX + 40, posY + 6, posX + 40, posY + TAMANO_TILE - 6);

                // Highlight (luz)
                painter->setPen(QColor(120, 85, 50));
                painter->drawLine(posX + 7, posY + 7, posX + 25, posY + 7);
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

            case LAMPARA:
                // Pared
                painter->fillRect(posX, posY, TAMANO_TILE, TAMANO_TILE,
                                  QColor(95, 100, 110));

                // Cable de la lámpara
                painter->setPen(QPen(QColor(40, 40, 45), 2));
                painter->drawLine(posX + 32, posY, posX + 32, posY + 20);

                // Lámpara (círculo amarillo brillante)
                painter->setBrush(QColor(255, 230, 120));
                painter->setPen(Qt::NoPen);
                painter->drawEllipse(posX + 22, posY + 15, 20, 20);

                // Brillo de la luz (halo)
                painter->setBrush(QColor(255, 240, 150, 60));
                painter->drawEllipse(posX + 17, posY + 10, 30, 30);
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
    if (camaraX > ANCHO_NIVEL - ANCHO_PANTALLA) {
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
