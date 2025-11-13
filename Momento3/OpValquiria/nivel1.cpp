#include "nivel1.h"
#include <QPainter>
#include <QDebug>

// Tipos de tiles para el mapa
enum TipoTile {
    VACIO = 0,      // Exterior/cielo
    PISO = 1,       // Suelo del cuartel
    PARED = 2,      // Paredes
    CAJA = 3,       // Cajas para decoración
    SOMBRA = 4,     // Zonas oscuras
    PUERTA = 5,     // Puertas
    VENTANA = 6,    // Ventanas
    LAMPARA = 7     // Lámparas
};

Nivel1::Nivel1()
    : Nivel(1),
    camaraX(0)
{
}

Nivel1::~Nivel1()
{
}

void Nivel1::inicializar()
{
    qDebug() << "Inicializando Nivel 1...";
    crearMapa();
    qDebug() << "Mapa creado!";
    completado = false;
}

void Nivel1::crearMapa()
{
    // Inicializar todo como VACIO (exterior)
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
    for (int y = 8; y < ALTO_MAPA; y++)
    {
        for (int x = 0; x < ANCHO_MAPA; x++)
        {
            mapa[y][x] = PISO;
        }
    }

    // Techo/pared superior (fila 7)
    for (int x = 0; x < ANCHO_MAPA; x++)
    {
        mapa[7][x] = PARED;
    }

    // ==========================================
    // HABITACIÓN 1: ENTRADA (columnas 0-12)
    // ==========================================
    for (int y = 8; y < 12; y++)
    {
        mapa[y][0] = PARED;   // Pared izquierda
        if (y != 9 && y != 10)
        {  // Dejar espacio para puerta
            mapa[y][12] = PARED;  // Pared derecha
        }
    }

    // Puerta de entrada
    mapa[9][12] = PUERTA;
    mapa[10][12] = PUERTA;

    // Decoración: caja en la esquina
    mapa[10][2] = CAJA;

    // Lámpara en el techo
    mapa[7][6] = LAMPARA;


    // ==========================================
    // PASILLO CENTRAL (columnas 12-28)
    // ==========================================
    // Solo piso, sin paredes laterales

    // Zona de sombra en el pasillo
    for (int x = 18; x < 22; x++)
    {
        mapa[10][x] = SOMBRA;
        mapa[11][x] = SOMBRA;
    }

    // Cajas en el pasillo para ocultarse
    mapa[9][15] = CAJA;
    mapa[9][25] = CAJA;

    // Lámparas del pasillo
    mapa[7][16] = LAMPARA;
    mapa[7][24] = LAMPARA;


    // ==========================================
    // HABITACIÓN 2: OFICINAS (columnas 28-40)
    // ==========================================
    for (int y = 8; y < 12; y++) {
        if (y != 9 && y != 10) {
            mapa[y][28] = PARED;  // Pared izquierda
        }
        mapa[y][40] = PARED;      // Pared derecha
    }

    // Puerta de entrada a oficinas
    mapa[9][28] = PUERTA;
    mapa[10][28] = PUERTA;

    // Ventanas en la pared superior
    mapa[7][32] = VENTANA;
    mapa[7][36] = VENTANA;

    // Mobiliario de oficina (cajas representan escritorios)
    mapa[9][30] = CAJA;
    mapa[9][34] = CAJA;
    mapa[9][38] = CAJA;

    // Zona de sombra bajo escritorio
    for (int x = 35; x < 38; x++) {
        mapa[10][x] = SOMBRA;
    }

    // Lámpara
    mapa[7][34] = LAMPARA;


    // ==========================================
    // HABITACIÓN 3: SALA DE REUNIONES - OBJETIVO (columnas 40-50)
    // ==========================================
    for (int y = 8; y < 12; y++) {
        if (y != 9 && y != 10) {
            mapa[y][40] = PARED;  // Pared izquierda
        }
        mapa[y][49] = PARED;      // Pared derecha
    }

    // Puerta de entrada
    mapa[9][40] = PUERTA;
    mapa[10][40] = PUERTA;

    // Mesa de reuniones (representada con cajas)
    for (int x = 43; x < 47; x++) {
        mapa[9][x] = CAJA;
    }

    // Ventanas
    mapa[7][44] = VENTANA;
    mapa[7][46] = VENTANA;

    // Lámpara central
    mapa[7][45] = LAMPARA;
}

void Nivel1::actualizar(float dt)
{
    // Por ahora solo dejamos que pase el tiempo
    // La cámara se moverá manualmente con las flechas
}

void Nivel1::renderizar(QPainter* painter)
{
    qDebug() << "Renderizando nivel... CamaraX:" << camaraX;
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
                                  QColor(15, 20, 35));
                break;

            case PISO:
                // Piso del cuartel - gris con textura
                painter->fillRect(posX, posY, TAMANO_TILE, TAMANO_TILE,
                                  QColor(55, 60, 70));

                // Líneas de baldosas
                painter->setPen(QColor(45, 50, 60));
                painter->drawRect(posX, posY, TAMANO_TILE, TAMANO_TILE);

                // Detalles de desgaste (pequeños rectángulos)
                if ((x + y) % 7 == 0)
                {
                    painter->fillRect(posX + 10, posY + 10, 8, 8,
                                      QColor(45, 50, 60));
                }
                break;

            case PARED:
                // Pared - gris claro con efecto de profundidad
                painter->fillRect(posX, posY, TAMANO_TILE, TAMANO_TILE,
                                  QColor(95, 100, 110));

                // Borde superior más claro (luz)
                painter->fillRect(posX, posY, TAMANO_TILE, 8,
                                  QColor(115, 120, 130));

                // Borde inferior más oscuro (sombra)
                painter->fillRect(posX, posY + TAMANO_TILE - 10,
                                  TAMANO_TILE, 10, QColor(65, 70, 80));

                // Líneas verticales (tablones)
                painter->setPen(QColor(85, 90, 100));
                for (int i = 0; i < TAMANO_TILE; i += 16) {
                    painter->drawLine(posX + i, posY, posX + i, posY + TAMANO_TILE);
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
