#include "nivel2.h"
#include <QPainter>

Nivel2::Nivel2()
    : Nivel(2),
    maletin(nullptr),
    tiempoRestante(90),
    errores(0),
    pasoActual(0),
    piezaSeleccionada(nullptr),
    mousePresionado(false)
{
    sonidoCorrecto.setSource(QUrl("qrc:/sounds//click_ok.mp3"));
    sonidoCorrecto.setVolume(0.8f);

    sonidoError.setSource(QUrl("qrc:/sounds/click_ok.mp3"));
    sonidoError.setVolume(0.8f);
}

Nivel2::~Nivel2() {
    for (auto p : piezas) {
        delete p;
    }
    piezas.clear();

    if (maletin) {
        delete maletin;
        maletin = nullptr;
    }
}
void Nivel2::inicializar() {
    completado = false;
    tiempoRestante = 90;
    errores = 0;
    piezaSeleccionada = nullptr;
    mousePresionado = false;
    pasoActual = 0;

    // MUY IMPORTANTE: limpiar si se reutiliza el nivel
    for (auto p : piezas) delete p;
    piezas.clear();
    delete maletin;
    maletin = nullptr;

    // Crear maletín en el centro de la pantalla (valores aproximados)
    QRectF areaMaletin(500, 200, 500, 300);
    maletin = new Maletin(areaMaletin);

    // Posiciones iniciales y objetivos aproximados
    piezas.push_back(new Pieza(
        TipoPieza::DETONADOR,
        QPointF(200, 250),
        QPointF(600, 300),
        40.0f
        ));

    piezas.push_back(new Pieza(
        TipoPieza::CARGA_EXPLOSIVA,
        QPointF(200, 350),
        QPointF(750, 300),
        40.0f
        ));

    piezas.push_back(new Pieza(
        TipoPieza::TEMPORIZADOR,
        QPointF(200, 450),
        QPointF(900, 300),
        40.0f
        ));
}

void Nivel2::actualizar(float dt) {
    static float acumulado = 0.0f;
    acumulado += dt;
    if (acumulado >= 1.0f) {
        if (tiempoRestante > 0) {
            tiempoRestante--;
        }
        acumulado = 0.0f;
    }

    // No dejamos que baje de 0
    if (tiempoRestante < 0) {
        tiempoRestante = 0;
    }

    // completado solo lo maneja verificarCondicionVictoria()
}

void Nivel2::renderizar(QPainter* painter) {
    // Fondo del escritorio
    painter->fillRect(0, 0, 1550, 770, QColor(50, 50, 60));

    // Dibujar maletín
    if (maletin) {
        maletin->dibujar(painter);
    }

    // Dibujar piezas
    for (auto p : piezas) {
        p->dibujar(painter);
    }

    // HUD de tiempo y errores
    if (tiempoRestante > 60) {
        painter->setPen(Qt::green);
    } else if (tiempoRestante > 30) {
        painter->setPen(Qt::yellow);
    } else {
        painter->setPen(Qt::red);
    }
    painter->setFont(QFont("Arial", 20, QFont::Bold));
    painter->drawText(20, 40, QString("Tiempo: %1 s").arg(tiempoRestante));

    // Errores en blanco
    painter->setPen(Qt::white);
    painter->setFont(QFont("Arial", 16));
    painter->drawText(20, 70, QString("Errores: %1").arg(errores));

    painter->drawText(20, 110,
                      "Arrastra las piezas al maletín.\n"
                      "Click derecho en el maletín para cerrarlo (por ahora lo haremos simple).");
}

bool Nivel2::verificarCondicionVictoria() {
    // Debe estar el maletín completo y cerrado y aún quedar tiempo
    return (tiempoRestante > 0) &&
           maletin &&
           maletin->estaCompleto() &&
           maletin->cerrado;
}

bool Nivel2::verificarCondicionDerrota() {
    // Derrota si se acaba el tiempo o demasiados errores
    return (tiempoRestante <= 0) || (errores >= 3);
}

void Nivel2::manejarMousePress(const QPointF& pos) {
    mousePresionado = true;
    piezaSeleccionada = nullptr;

    // Ver si se hizo click en alguna pieza
    for (auto p : piezas) {
        if (!p->colocada && p->contienePunto(pos)) {
            piezaSeleccionada = p;
            p->iniciarArrastre();
            return;
        }
    }

    // Si clic derecho dentro del maletín, intentamos cerrarlo (simplificado)
    // NOTA: esto lo puedes mejorar luego.
    // Aquí asumo que el botón ya se manejó en Juego (si quisieras)
}

void Nivel2::manejarMouseMove(const QPointF& pos) {
    if (mousePresionado && piezaSeleccionada) {
        piezaSeleccionada->moverA(pos);
    }
}

void Nivel2::manejarMouseRelease(const QPointF& pos) {
    Q_UNUSED(pos);
    mousePresionado = false;

    if (piezaSeleccionada) {
        bool ok = piezaSeleccionada->soltar();  // ¿encajó (radio correcto)?

        if (ok) {
            // Verificar si se respetó el ORDEN correcto
            bool ordenCorrecto = false;
            switch (pasoActual) {
            case 0: // primero debe ir el DETONADOR
                ordenCorrecto = (piezaSeleccionada->tipo == TipoPieza::DETONADOR);
                break;
            case 1: // segundo, la CARGA
                ordenCorrecto = (piezaSeleccionada->tipo == TipoPieza::CARGA_EXPLOSIVA);
                break;
            case 2: // tercero, el TEMPORIZADOR
                ordenCorrecto = (piezaSeleccionada->tipo == TipoPieza::TEMPORIZADOR);
                break;
            }

            if (ordenCorrecto) {
                // Pieza correcta en el orden correcto
                if (maletin) {
                    maletin->agregarPieza();
                }
                tiempoRestante += 2;  // bonus por colocación correcta
                errores = 0;          // reiniciar errores consecutivos
                pasoActual++;         // pasamos al siguiente paso
                qDebug() << "SONIDO CORRECTO";
                sonidoCorrecto.play();
            } else {
                // Orden INCORRECTO: penalización
                errores++;
                tiempoRestante -= 5;

                // deshacemos el "encaje"
                piezaSeleccionada->colocada = false;
                piezaSeleccionada->regresarPosicionInicial();
                qDebug() << "SONIDO ERROR (orden)";
                sonidoError.play();
            }
        } else {
            // Pieza mal colocada (fuera de la zona de encaje)
            errores++;
            tiempoRestante -= 5;
            piezaSeleccionada->regresarPosicionInicial();
            qDebug() << "SONIDO ERROR (orden)";
            sonidoError.play();
        }

        piezaSeleccionada = nullptr;
    }

    // Si todas las piezas se han colocado en orden, cerramos el maletín
    if (maletin && maletin->estaCompleto() && !maletin->cerrado) {
        maletin->cerrar();
    }
}
