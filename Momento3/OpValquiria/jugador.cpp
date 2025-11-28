#include "jugador.h"
#include <QPainter>
#include <QDebug>

Jugador::Jugador(float x, float y)
    : Entidad(x, y, 32, 48),  // Tamaño: 32x48 píxeles
    agachado(false),
    corriendo(false),
    oculto(false), muerto(false),
    direccionActual(Direccion::DOWN),
    direccionMuerte(Direccion::DOWN),
    teclaIzquierda(false),
    teclaDerecha(false),
    teclaArriba(false),
    teclaAbajo(false),
    teclaShift(false),
    teclaC(false),
    tiempoAnimacion(0),
    frameActual(0)
{
    qDebug() << "Jugador creado en posición:" << x << "," << y;

    sprites.resize(8);

    nombresSpritesU << ":/sprites/sprites/GeneralU1.PNG" << ":/sprites/sprites/GeneralU2.PNG" << ":/sprites/sprites/GeneralU3.PNG" << ":/sprites/sprites/GeneralU4.PNG";
    nombresSpritesD << ":/sprites/sprites/GeneralD1.PNG" << ":/sprites/sprites/GeneralD2.PNG" << ":/sprites/sprites/GeneralD3.PNG" << ":/sprites/sprites/GeneralD4.PNG";
    nombresSpritesL << ":/sprites/sprites/GeneralL1.PNG" << ":/sprites/sprites/GeneralL2.PNG" << ":/sprites/sprites/GeneralL3.PNG" << ":/sprites/sprites/GeneralL4.PNG";
    nombresSpritesR << ":/sprites/sprites/GeneralR1.PNG" << ":/sprites/sprites//GeneralR2.PNG" << ":/sprites/sprites/GeneralR3.PNG" << ":/sprites/sprites/GeneralR4.PNG";
    nombresSpritesAU << ":/sprites/sprites/GeneralagachadoU1.PNG" << ":/sprites/sprites/GeneralagachadoU2.PNG";
    nombresSpritesAD << ":/sprites/sprites/GeneralagachadoD1.PNG" << ":/sprites/sprites/GeneralagachadoD2.PNG";
    nombresSpritesAL << ":/sprites/sprites/GeneralagachadoL1.PNG" << ":/sprites/sprites/GeneralagachadoL2.PNG";
    nombresSpritesAR << ":/sprites/sprites/GeneralagachadoR1.PNG" << ":/sprites/sprites/GeneralagachadoR2.PNG";

    cargarSprites();
}

Jugador::~Jugador()
{
}

void Jugador::cargarSprites()
{
    qDebug() << "=== CARGANDO SPRITES DEL JUGADOR ===";

    // DIRECCIÓN UP (índice 0)
    qDebug() << "Cargando sprites UP:";
    for (int i = 0; i < nombresSpritesU.size(); i++)
    {
        QPixmap sprite = QPixmap(nombresSpritesU[i]);
        sprites[static_cast<int>(Direccion::UP)].append(sprite);
        qDebug() << "  " << nombresSpritesU[i] << ":" << !sprite.isNull();
    }

    // DIRECCIÓN DOWN (índice 1)
    qDebug() << "Cargando sprites DOWN:";
    for (int i = 0; i < nombresSpritesD.size(); i++)
    {
        QPixmap sprite = QPixmap(nombresSpritesD[i]);
        sprites[static_cast<int>(Direccion::DOWN)].append(sprite);
        qDebug() << "  " << nombresSpritesD[i] << ":" << !sprite.isNull();
    }

    // DIRECCIÓN IZQUIERDA (índice 2)
    qDebug() << "Cargando sprites LEFT:";
    for (int i = 0; i < nombresSpritesL.size(); i++)
    {
        QPixmap sprite = QPixmap(nombresSpritesL[i]);
        sprites[static_cast<int>(Direccion::LEFT)].append(sprite);
        qDebug() << "  " << nombresSpritesL[i] << ":" << !sprite.isNull();
    }

    // DIRECCIÓN DERECHA (índice 3)
    qDebug() << "Cargando sprites RGTH:";
    for (int i = 0; i < nombresSpritesR.size(); i++)
    {
        QPixmap sprite = QPixmap(nombresSpritesR[i]);
        sprites[static_cast<int>(Direccion::RIGTH)].append(sprite);
        qDebug() << "  " << nombresSpritesR[i] << ":" << !sprite.isNull();
    }

    //DIRECCIÓN Agachado Up (índice 4)
    for (int i = 0; i < nombresSpritesAU.size(); i++)
    {
        QPixmap sprite = QPixmap(nombresSpritesAU[i]);
        sprites[static_cast<int>(Direccion::AUP)].append(sprite);
        qDebug() << "  " << nombresSpritesAU[i] << ":" << !sprite.isNull();
    }

    // DIRECCIÓN Agachado abajo (índice 5)
    for (int i = 0; i < nombresSpritesAD.size(); i++)
    {
        QPixmap sprite = QPixmap(nombresSpritesAD[i]);
        sprites[static_cast<int>(Direccion::ADOWN)].append(sprite);
        qDebug() << "  " << nombresSpritesAD[i] << ":" << !sprite.isNull();
    }

    // DIRECCIÓN Agachado izquierda (índice 6)
    for (int i = 0; i < nombresSpritesAL.size(); i++)
    {
        QPixmap sprite = QPixmap( nombresSpritesAL[i]);
        sprites[static_cast<int>(Direccion::ALEFT)].append(sprite);
        qDebug() << "  " << nombresSpritesAL[i] << ":" << !sprite.isNull();
    }

    // DIRECCIÓN Agachado derecha (índice 7)
    for (int i = 0; i < nombresSpritesAR.size(); i++)
    {
        QPixmap sprite = QPixmap(nombresSpritesAR[i]);
        sprites[static_cast<int>(Direccion::ARIGTH)].append(sprite);
        qDebug() << "  " << nombresSpritesAR[i] << ":" << !sprite.isNull();
    }

    // SPRITE Muerte
    spriteMuerteU = QPixmap(":/sprites/sprites/GeneralMuerteU.png");
    spriteMuerteD = QPixmap(":/sprites/sprites/GeneralMuerteD.png");
    spriteMuerteL = QPixmap(":/sprites/sprites/GeneralMuerteL.png");
    spriteMuerteR = QPixmap(":/sprites/sprites/GeneralMuerteR.png");


    qDebug() << "=== FIN CARGA SPRITES JUGADOR ===";
}
void Jugador::morir()
{
    if (muerto) return;  // Ya está muerto

    muerto = true;
    tiempoMuerte = 0.0f;
    velocidadX = 0;
    velocidadY = 0;

    // Guardar la dirección actual para el sprite de muerte
    if (direccionActual == Direccion::UP || direccionActual == Direccion::AUP)
    {
        direccionMuerte = Direccion::UP;
    }
    else if (direccionActual == Direccion::DOWN || direccionActual == Direccion::ADOWN)
    {
        direccionMuerte = Direccion::DOWN;
    }
    else if (direccionActual == Direccion::LEFT || direccionActual == Direccion::ALEFT)
    {
        direccionMuerte = Direccion::LEFT;
    }
    else
    {
        direccionMuerte = Direccion::RIGTH;
    }

    qDebug() << "¡JUGADOR MUERTO!";
}
int Jugador::getNumFrames(Direccion dir) const
{
    int dirIndex = static_cast<int>(dir);
    if (dirIndex >= 0 && dirIndex < sprites.size())
    {
        return sprites[dirIndex].size();
    }
    return 0;
}
void Jugador::actualizar(float dt)
{
    if (!activo) return;

    if (muerto)
    {
        tiempoMuerte += dt;
        return;
    }

    // Resetear velocidad
    velocidadX = 0;
    velocidadY = 0;

    // Determinar velocidad según estado
    float velocidadActual = VEL_NORMAL;

    if (teclaC && !corriendo)
    {
        agachado = true;
        velocidadActual = VEL_AGACHADO;
    } else {
        agachado = false;
    }

    if (teclaShift && !agachado)
    {
        corriendo = true;
        velocidadActual = VEL_CORRIENDO;
    } else {
        corriendo = false;
    }
    bool moviendose = false;
    // Movimiento horizontal
    if (teclaIzquierda)
    {
        velocidadX = -velocidadActual;
        direccionActual = Direccion::LEFT;
        moviendose = true;
    }
    else if (teclaDerecha)
    {
        velocidadX = velocidadActual;
        direccionActual = Direccion::RIGTH;
        moviendose = true;
    }
    else if (teclaArriba)
    {
        velocidadY = -velocidadActual;
        direccionActual = Direccion::UP;
        moviendose = true;
    }
    else if (teclaAbajo)
    {
        velocidadY = velocidadActual;
        direccionActual = Direccion::DOWN;
        moviendose = true;
    }
    if (teclaIzquierda && agachado)
    {
        velocidadX = -velocidadActual;
        direccionActual = Direccion::ALEFT;
        moviendose = true;
    }
    else if (teclaDerecha && agachado)
    {
        velocidadX = velocidadActual;
        direccionActual = Direccion::ARIGTH;
        moviendose = true;
    }
    else if (teclaArriba && agachado)
    {
        velocidadY = -velocidadActual;
        direccionActual = Direccion::AUP;
        moviendose = true;
    }
    else if (teclaAbajo && agachado)
    {
        velocidadY = velocidadActual;
        direccionActual = Direccion::ADOWN;
        moviendose = true;
    }
    // Aplicar movimiento
    x += velocidadX * dt;
    y += velocidadY * dt;

    // Limitar a los bordes del nivel
    // Ancho del nivel: 50 tiles * 64 = 3200 píxeles
    // Alto del nivel: 12 tiles * 64 = 768 píxeles

    if (x < 0) x = 0;
    if (x > 3200 - ancho) x = 3200 - ancho;
    if (y < 0) y = 0;
    if (y > 768 - alto) y = 768 - alto;

    // Actualizar animación
    if (moviendose)
    {
        tiempoAnimacion += dt;

        float velocidadAnimacion = 0.15f;
        if (corriendo)
        {
            velocidadAnimacion = 0.06f;
        }
        else if (agachado)
        {
            velocidadAnimacion = 0.3f;
        }

        if (tiempoAnimacion > velocidadAnimacion)
        {
            tiempoAnimacion = 0;
            int numFrames = getNumFrames(direccionActual);
            if (numFrames > 0)
            {
                frameActual = (frameActual + 1) % numFrames;
            }
        }
    }
    else
    {
        frameActual = 0;  // Frame idle
        tiempoAnimacion = 0;
    }
}

void Jugador::renderizar(QPainter* painter)
{
    if (!activo || !painter) return;

    if (muerto)
    {
        QPixmap spriteMuerte;

        switch (direccionMuerte)
        {
        case Direccion::UP:
            spriteMuerte = spriteMuerteU;
            break;
        case Direccion::DOWN:
            spriteMuerte = spriteMuerteD;
            break;
        case Direccion::LEFT:
            spriteMuerte = spriteMuerteL;
            break;
        case Direccion::RIGTH:
            spriteMuerte = spriteMuerteR;
            break;
        default:
            spriteMuerte = spriteMuerteD;
            break;
        }
    }
    QPixmap spriteActual;


    // Obtener índice de dirección
    int dirIndex = static_cast<int>(direccionActual);

    // Verificar que la dirección y frame sean válidos
    if (dirIndex >= 0 && dirIndex < sprites.size())
    {
        if (frameActual >= 0 && frameActual < sprites[dirIndex].size())
        {
            spriteActual = sprites[dirIndex][frameActual];
        }
    }

    if (!spriteActual.isNull())
    {
        painter->drawPixmap(x, y, ancho, alto, spriteActual);
    }
    else
    {
        // Fallback: Rectángulo de colores (si no hay sprite)
        QColor colorJugador;

        if (agachado)
        {
            colorJugador = QColor(100, 150, 100);
        }
        else if (corriendo)
        {
            colorJugador = QColor(150, 200, 150);
        }
        else
        {
            colorJugador = QColor(120, 180, 120);
        }

        painter->fillRect(x, y, ancho, alto, colorJugador);
        painter->setPen(QPen(Qt::black, 2));
        painter->drawRect(x, y, ancho, alto);

        // Indicador de dirección
        painter->setPen(Qt::white);
        painter->setFont(QFont("Arial", 10, QFont::Bold));

        QString direccion;
        switch (direccionActual)
        {
        case Direccion::DOWN: direccion = "↓"; break;
        case Direccion::UP: direccion = "↑"; break;
        case Direccion::LEFT: direccion = "←"; break;
        case Direccion::RIGTH: direccion = "→"; break;
        case Direccion::ADOWN: direccion = "↓"; break;
        case Direccion::AUP: direccion = "↑"; break;
        case Direccion::ALEFT: direccion = "←"; break;
        case Direccion::ARIGTH: direccion = "→"; break;

        }
        painter->drawText(x + 10, y + 28, direccion);

        // Mostrar que falta sprite
        painter->setPen(Qt::red);
        painter->setFont(QFont("Arial", 8));
        painter->drawText(x, y - 5, "NO SPRITE");
    }
}

void Jugador::manejarTeclaPresionada(int key)
{
    switch (key) {
    case Qt::Key_Left:
    case Qt::Key_A:
        teclaIzquierda = true;
        break;
    case Qt::Key_Right:
    case Qt::Key_D:
        teclaDerecha = true;
        break;
    case Qt::Key_Up:
    case Qt::Key_W:
        teclaArriba = true;
        break;
    case Qt::Key_Down:
    case Qt::Key_S:
        teclaAbajo = true;
        break;
    case Qt::Key_Shift:
        teclaShift = true;
        break;
    case Qt::Key_C:
        teclaC = true;
        break;
    }
}

void Jugador::manejarTeclaSoltada(int key)
{
    switch (key) {
    case Qt::Key_Left:
    case Qt::Key_A:
        teclaIzquierda = false;
        break;
    case Qt::Key_Right:
    case Qt::Key_D:
        teclaDerecha = false;
        break;
    case Qt::Key_Up:
    case Qt::Key_W:
        teclaArriba = false;
        break;
    case Qt::Key_Down:
    case Qt::Key_S:
        teclaAbajo = false;
        break;
    case Qt::Key_Shift:
        teclaShift = false;
        corriendo = false;
        break;
    case Qt::Key_C:
        teclaC = false;
        agachado = false;
        break;
    }
}
