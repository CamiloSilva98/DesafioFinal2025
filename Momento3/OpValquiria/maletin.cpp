#include "maletin.h"

#include <QFont>
#include <QString>
#include <QDebug>
#include <QPen>

Maletin::Maletin(const QRectF& rect)
    : area(rect),
    cerrado(false),
    piezasNecesarias(3),
    piezasColocadas(0)
{
    sprite.load(":/sprites/sprites/maletin.png");
    if (sprite.isNull()) {
        qDebug() << "ERROR: No se pudo cargar sprite del maletín";
    }
}

void Maletin::agregarPieza() {
    if (piezasColocadas < piezasNecesarias) {
        ++piezasColocadas;
    }
}

bool Maletin::estaCompleto() const {
    return piezasColocadas >= piezasNecesarias;
}

void Maletin::cerrar() {
    if (estaCompleto()) {
        cerrado = true;
    }
}

void Maletin::dibujar(QPainter* painter) {
    painter->save();

    // Fondo base del maletín: tono verde/gris militar
    QColor baseColor(60, 75, 60);    // tono sobrio
    QColor innerColor(75, 95, 75);   // un poco más claro para interior

    // Marco externo
    painter->setBrush(baseColor);
    painter->setPen(QPen(Qt::black, 4));
    painter->drawRoundedRect(area.adjusted(-4, -4, 4, 4), 12, 12);

    // Interior
    QRectF inner = area.adjusted(8, 8, -8, -8);
    painter->setBrush(innerColor);
    painter->setPen(QPen(Qt::black, 2));
    painter->drawRoundedRect(inner, 10, 10);

    // Sprite del maletín encima (si existe)
    if (!sprite.isNull()) {
        painter->drawPixmap(inner.toRect(), sprite);
    }

    // Texto de progreso
    painter->setPen(Qt::white);
    painter->setFont(QFont("Arial", 16, QFont::Bold));

    const QString texto = QString("%1 / %2")
                              .arg(piezasColocadas)
                              .arg(piezasNecesarias);

    painter->drawText(inner.adjusted(10, 10, -10, -10),
                      Qt::AlignTop | Qt::AlignLeft,
                      texto);

    if (cerrado) {
        painter->setPen(QColor(120, 255, 120));
        painter->setFont(QFont("Arial", 26, QFont::Bold));
        painter->drawText(inner, Qt::AlignCenter, "¡CERRADO!");
    }

    painter->restore();
}
