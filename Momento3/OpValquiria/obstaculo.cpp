#include "obstaculo.h"
#include <QPainter>
#include <QDebug>

Obstaculo::Obstaculo(float x, float y, TipoObstaculo tipo, float velocidadScroll)
    : Entidad(x, y, 64, 64),  // Tamaño por defecto
    tipo(tipo),
    velocidadScroll(velocidadScroll),
    solido(true)
{
    // Ajustar tamaño según tipo
    switch (tipo) {
    case TipoObstaculo::VALLA:
        ancho = 40;
        alto = 60;
        break;
    case TipoObstaculo::BARRERA_BAJA:
        ancho = 60;
        alto = 30;
        break;
    case TipoObstaculo::GUARDIA:
        ancho = 32;
        alto = 48;
        break;
    case TipoObstaculo::NUBE:
        ancho = 100;
        alto = 60;
        solido = false;  // Las nubes no causan colisión
        break;
    case TipoObstaculo::TORRE:
        ancho = 50;
        alto = 150;
        break;
    case TipoObstaculo::GLOBO:
        ancho = 60;
        alto = 80;
        break;
    case TipoObstaculo::AVION_ENEMIGO:
        ancho = 70;
        alto = 35;
        break;
    }

    cargarSprite();
}

Obstaculo::~Obstaculo()
{
}

void Obstaculo::cargarSprite()
{
    QString rutaSprite;

    switch (tipo) {
    case TipoObstaculo::VALLA:
        rutaSprite = ":/sprites/sprites/valla.png";
        break;
    case TipoObstaculo::BARRERA_BAJA:
        rutaSprite = ":/sprites/sprites/barrera.png";
        break;
    case TipoObstaculo::GUARDIA:
        rutaSprite = ":/sprites/sprites/GuardiaD1.png";
        break;
    case TipoObstaculo::NUBE:
        rutaSprite = ":/sprites/sprites/nube.png";
        break;
    case TipoObstaculo::TORRE:
        rutaSprite = ":/sprites/sprites/torre.png";
        break;
    case TipoObstaculo::GLOBO:
        rutaSprite = ":/sprites/sprites/globo.png";
        break;
    case TipoObstaculo::AVION_ENEMIGO:
        rutaSprite = ":/sprites/sprites/avion_enemigo.png";
        break;
    }

    sprite = QPixmap(rutaSprite);

    if (sprite.isNull()) {
        qDebug() << "Advertencia: No se pudo cargar sprite de obstáculo:" << rutaSprite;
    }
}

void Obstaculo::actualizar(float dt)
{
    if (!activo) return;

    // Mover hacia la izquierda (simular scroll)
    x -= velocidadScroll * dt;

    // Desactivar si salió de pantalla
    if (x < -ancho) {
        activo = false;
    }
}

void Obstaculo::renderizar(QPainter* painter)
{
    if (!activo || !painter) return;

    if (!sprite.isNull()) {
        painter->drawPixmap(x, y, ancho, alto, sprite);
    } else {
        // Fallback: Dibujar según tipo
        switch (tipo) {
        case TipoObstaculo::VALLA:
            // Valla marrón
            painter->fillRect(x, y, ancho, alto, QColor(139, 69, 19));
            painter->setPen(QPen(QColor(101, 67, 33), 3));
            painter->drawLine(x + 10, y, x + 10, y + alto);
            painter->drawLine(x + 20, y, x + 20, y + alto);
            painter->drawLine(x + 30, y, x + 30, y + alto);
            break;

        case TipoObstaculo::BARRERA_BAJA:
            // Barrera roja y blanca
            painter->fillRect(x, y, ancho/2, alto, Qt::red);
            painter->fillRect(x + ancho/2, y, ancho/2, alto, Qt::white);
            painter->setPen(QPen(Qt::black, 2));
            painter->drawRect(x, y, ancho, alto);
            break;

        case TipoObstaculo::GUARDIA:
            // Guardia simple (rectángulo azul)
            painter->fillRect(x, y, ancho, alto, QColor(100, 100, 200));
            painter->setPen(QPen(Qt::black, 2));
            painter->drawRect(x, y, ancho, alto);
            break;

        case TipoObstaculo::NUBE:
            // Nube blanca (varios círculos)
            painter->setBrush(QColor(255, 255, 255, 150));
            painter->setPen(Qt::NoPen);
            painter->drawEllipse(x, y + 20, 40, 40);
            painter->drawEllipse(x + 20, y + 10, 50, 50);
            painter->drawEllipse(x + 40, y + 20, 40, 40);
            break;

        case TipoObstaculo::TORRE:
            // Torre gris
            painter->fillRect(x, y, ancho, alto, QColor(100, 100, 100));
            painter->setPen(QPen(Qt::black, 3));
            painter->drawRect(x, y, ancho, alto);

            // Ventanas
            painter->fillRect(x + 10, y + 20, 15, 15, QColor(255, 255, 0));
            painter->fillRect(x + 25, y + 20, 15, 15, QColor(255, 255, 0));
            painter->fillRect(x + 10, y + 50, 15, 15, QColor(255, 255, 0));
            painter->fillRect(x + 25, y + 50, 15, 15, QColor(255, 255, 0));
            break;

        case TipoObstaculo::GLOBO:
            // Globo aerostático
            painter->setBrush(QColor(255, 100, 100));
            painter->setPen(QPen(Qt::black, 2));
            painter->drawEllipse(x, y, ancho, alto - 20);

            // Cesta
            painter->fillRect(x + 20, y + alto - 20, 20, 20, QColor(139, 69, 19));

            // Cuerdas
            painter->drawLine(x + 15, y + alto - 20, x + 25, y + alto - 25);
            painter->drawLine(x + 45, y + alto - 20, x + 35, y + alto - 25);
            break;

        case TipoObstaculo::AVION_ENEMIGO:
            // Avión enemigo (rojo)
            painter->setBrush(QColor(200, 50, 50));
            painter->setPen(QPen(Qt::black, 2));

            QPolygonF triangulo;
            triangulo << QPointF(x, y + alto/2)           // Punta izquierda
                      << QPointF(x + ancho, y)            // Arriba derecha
                      << QPointF(x + ancho, y + alto);    // Abajo derecha

            painter->drawPolygon(triangulo);
            break;
        }
    }

// Debug: Hitbox si es sólido
#ifdef DEBUG_COLISIONES
    if (solido) {
        painter->setPen(QPen(Qt::red, 1, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(x, y, ancho, alto);
    }
#endif
}
