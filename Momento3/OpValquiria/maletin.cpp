#include "maletin.h"

Maletin::Maletin(const QRectF& rect)
    : area(rect),
    cerrado(false),
    piezasNecesarias(3),
    piezasColocadas(0)
{
}

void Maletin::agregarPieza() {
    if (piezasColocadas < piezasNecesarias) {
        piezasColocadas++;
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

    // Fondo del maletín
    painter->setBrush(QColor(80, 80, 80));
    painter->setPen(Qt::white);
    painter->drawRect(area);

    // Indicar si está cerrado
    painter->setPen(Qt::white);
    painter->setFont(QFont("Arial", 12));

    QString texto = QString("Maletín - Piezas: %1/%2")
                        .arg(piezasColocadas)
                        .arg(piezasNecesarias);
    painter->drawText(area.adjusted(5, 5, -5, -5), Qt::AlignTop | Qt::AlignLeft, texto);

    if (cerrado) {
        painter->setPen(Qt::green);
        painter->drawText(area, Qt::AlignCenter, "CERRADO");
    }

    painter->restore();
}
