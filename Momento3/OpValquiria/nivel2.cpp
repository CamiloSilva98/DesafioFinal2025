#include "nivel2.h"

#include <QPainter>
#include <QDebug>
#include <QFont>
#include <QPen>
#include <QRandomGenerator>

Nivel2::Nivel2()
    : Nivel(2),
    maletin(nullptr),
    tiempoRestante(90),
    errores(0),
    pasoActual(0),
    piezaSeleccionada(nullptr),
    mousePresionado(false),
    mostrarLibro(false)
{
    // Cargar sprite del libro (opcional)
    libroSprite.load(":/sprites/sprites/carta.png");
    if (libroSprite.isNull()) {
        qDebug() << "ADVERTENCIA: No se pudo cargar sprite de la carta";
    }

    // Cargar sprite del fondo
    fondoSprite.load(":/sprites/sprites/fondo2.jpeg");
    if (fondoSprite.isNull()) {
        qDebug() << "ADVERTENCIA: No se pudo cargar sprite del fondo";
    }
}

Nivel2::~Nivel2() {
    for (Pieza* p : piezas) {
        delete p;
    }
    piezas.clear();

    delete maletin;
    maletin = nullptr;
}

void Nivel2::inicializar() {
    completado        = false;
    tiempoRestante    = 90;
    errores           = 0;
    piezaSeleccionada = nullptr;
    mousePresionado   = false;
    pasoActual        = 0;

    // Cada vez que entras al nivel se muestra el libro con la historia/pistas
    mostrarLibro = true;

    // Limpiar piezas anteriores
    for (Pieza* p : piezas) {
        delete p;
    }
    piezas.clear();

    delete maletin;
    maletin = nullptr;

    // ================== MALETÍN MÁS GRANDE Y CENTRADO ==================
    const QRectF areaMaletin(600.0, 180.0, 600.0, 420.0);
    maletin = new Maletin(areaMaletin);

    // ===== GENERACIÓN ALEATORIA MÁS COMPACTA A LA IZQUIERDA =====
    auto randomPosIzquierda = []() -> QPointF {
        // Zona izquierda, amplia pero bien separada del maletín
        const int xMin = 80;
        const int xMax = 380;   // todo a la izquierda
        const int yMin = 260;
        const int yMax = 640;

        const int x = QRandomGenerator::global()->bounded(xMin, xMax + 1);
        const int y = QRandomGenerator::global()->bounded(yMin, yMax + 1);

        return QPointF(static_cast<double>(x),
                       static_cast<double>(y));
    };

    // Posiciones objetivo dentro del maletín (3 columnas centradas)
    const double centerY = areaMaletin.center().y();          // centro vertical
    const double leftX   = areaMaletin.left() + areaMaletin.width() * 0.25;
    const double midX    = areaMaletin.left() + areaMaletin.width() * 0.50;
    const double rightX  = areaMaletin.left() + areaMaletin.width() * 0.75;

    const QPointF objetivoDetonador   (leftX,  centerY);
    const QPointF objetivoCarga       (midX,   centerY);
    const QPointF objetivoTemporizador(rightX, centerY);

    const float radioCorrectas = 80.0f;

    // ================== 3 PIEZAS CORRECTAS ==================
    piezas.push_back(new Pieza(
        TipoPieza::DETONADOR,
        randomPosIzquierda(),
        objetivoDetonador,
        radioCorrectas,
        true));

    piezas.push_back(new Pieza(
        TipoPieza::CARGA_EXPLOSIVA,
        randomPosIzquierda(),
        objetivoCarga,
        radioCorrectas,
        true));

    piezas.push_back(new Pieza(
        TipoPieza::TEMPORIZADOR,
        randomPosIzquierda(),
        objetivoTemporizador,
        radioCorrectas,
        true));

    // ================== PIEZAS RANDOM (DISTRACTORES) ==================
    const float  radioRandom      = 80.0f;
    const QPointF objetivoInvalido(-1000.0, -1000.0);

    // 6 distractores nada más
    piezas.push_back(new Pieza(TipoPieza::RANDOM1, randomPosIzquierda(), objetivoInvalido, radioRandom, false));
    piezas.push_back(new Pieza(TipoPieza::RANDOM2, randomPosIzquierda(), objetivoInvalido, radioRandom, false));
    piezas.push_back(new Pieza(TipoPieza::RANDOM3, randomPosIzquierda(), objetivoInvalido, radioRandom, false));
    piezas.push_back(new Pieza(TipoPieza::RANDOM4, randomPosIzquierda(), objetivoInvalido, radioRandom, false));
    piezas.push_back(new Pieza(TipoPieza::RANDOM5, randomPosIzquierda(), objetivoInvalido, radioRandom, false));
    piezas.push_back(new Pieza(TipoPieza::RANDOM6, randomPosIzquierda(), objetivoInvalido, radioRandom, false));
}

void Nivel2::actualizar(float dt) {
    static float acumulado = 0.0f;
    acumulado += dt;

    if (acumulado >= 1.0f) {
        if (tiempoRestante > 0) {
            --tiempoRestante;
        }
        acumulado = 0.0f;
    }

    if (tiempoRestante < 0) {
        tiempoRestante = 0;
    }
}

void Nivel2::renderizar(QPainter* painter) {
    // Dibujar fondo
    if (!fondoSprite.isNull()) {
        painter->drawPixmap(0, 0, 1600, 900, fondoSprite);
    } else {
        // Fallback: color café
        painter->fillRect(0, 0, 1600, 900, QColor(139, 90, 60));
    }

    if (maletin) {
        maletin->dibujar(painter);
    }

    for (Pieza* p : piezas) {
        p->dibujar(painter);
    }

    // HUD tiempo
    if (tiempoRestante > 60) {
        painter->setPen(Qt::green);
    } else if (tiempoRestante > 30) {
        painter->setPen(QColor(255, 200, 0));
    } else {
        painter->setPen(Qt::red);
    }

    painter->setFont(QFont("Arial", 22, QFont::Bold));
    painter->drawText(20, 40, QStringLiteral("Tiempo: %1 s").arg(tiempoRestante));

    // Errores
    painter->setPen(Qt::white);
    painter->setFont(QFont("Arial", 18));
    painter->drawText(20, 75, QStringLiteral("Errores: %1 / 3").arg(errores));

    // Instrucciones
    painter->setFont(QFont("Arial", 14));
    painter->drawText(20, 110,
                      QStringLiteral("Arrastra las piezas correctas al maletín en orden:\n"
                                     "1. Detonador  2. Carga Explosiva  3. Temporizador"));
    painter->drawText(20, 160,
                      QStringLiteral("El libro con pistas se muestra al entrar al nivel.\n"
                                     "Presiona L para abrir/cerrar el libro."));

    if (mostrarLibro) {
        dibujarLibro(painter);
    }
}

bool Nivel2::verificarCondicionVictoria() {
    return (tiempoRestante > 0) &&
           maletin &&
           maletin->estaCompleto() &&
           maletin->cerrado;
}

bool Nivel2::verificarCondicionDerrota() {
    return (tiempoRestante <= 0) || (errores >= 3);
}

void Nivel2::manejarMousePress(const QPointF& pos) {
    mousePresionado   = true;
    piezaSeleccionada = nullptr;

    // Buscar pieza más "arriba"
    for (int i = static_cast<int>(piezas.size()) - 1; i >= 0; --i) {
        Pieza* p = piezas[static_cast<std::size_t>(i)];
        if (!p->colocada && p->contienePunto(pos)) {
            piezaSeleccionada = p;
            piezaSeleccionada->iniciarArrastre();
            return;
        }
    }
}

void Nivel2::manejarMouseMove(const QPointF& pos) {
    if (mousePresionado && piezaSeleccionada) {
        piezaSeleccionada->moverA(pos);
    }
}

void Nivel2::manejarMouseRelease(const QPointF& pos) {
    Q_UNUSED(pos);
    mousePresionado = false;

    if (!piezaSeleccionada) {
        return;
    }

    // Intenta encajar (para las piezas correctas con objetivo)
    const bool encajo = piezaSeleccionada->soltar();

    // ¿Quedó dentro del área del maletín?
    bool dentroMaletin = false;
    if (maletin) {
        // Zona un poco más pequeña que el área total para que sea más natural
        QRectF zonaColocacion = maletin->area.adjusted(20, 20, -20, -20);
        dentroMaletin = zonaColocacion.contains(piezaSeleccionada->posicion);
    }

    if (piezaSeleccionada->esCorrecta) {
        // ------------ PIEZA CORRECTA ------------
        if (encajo && dentroMaletin) {
            // Encajó en su punto objetivo dentro del maletín:
            // aquí comprobamos el ORDEN.
            bool ordenCorrecto = false;

            switch (pasoActual) {
            case 0:
                ordenCorrecto = (piezaSeleccionada->tipo == TipoPieza::DETONADOR);
                break;
            case 1:
                ordenCorrecto = (piezaSeleccionada->tipo == TipoPieza::CARGA_EXPLOSIVA);
                break;
            case 2:
                ordenCorrecto = (piezaSeleccionada->tipo == TipoPieza::TEMPORIZADOR);
                break;
            default:
                break;
            }

            if (ordenCorrecto) {
                if (maletin) {
                    maletin->agregarPieza();
                }
                tiempoRestante += 2;   // BONUS
                errores = 0;           // resetea errores consecutivos
                ++pasoActual;
                qDebug() << "Pieza correcta en el orden correcto. +2s";
            } else {
                // Correcta pero en orden incorrecto
                ++errores;
                tiempoRestante -= 5;
                piezaSeleccionada->colocada = false;
                piezaSeleccionada->regresarPosicionInicial();
                qDebug() << "ERROR: Pieza correcta pero en orden incorrecto. -5s";
            }
        } else if (dentroMaletin) {
            // Está dentro del maletín pero NO encajó en su slot correcto
            // => intento fallido
            ++errores;
            tiempoRestante -= 5;
            piezaSeleccionada->colocada = false;
            piezaSeleccionada->regresarPosicionInicial();
            qDebug() << "ERROR: Pieza correcta mal colocada en el maletín. -5s";
        } else {
            // Fuera del maletín -> sin penalización
            piezaSeleccionada->regresarPosicionInicial();
            qDebug() << "Pieza correcta soltada fuera del maletín. Sin penalización.";
        }
    } else {
        // ------------ PIEZA RANDOM / DISTRACTOR ------------
        if (dentroMaletin) {
            // Cualquier random dentro del maletín es error
            ++errores;
            tiempoRestante -= 5;
            piezaSeleccionada->colocada = false;
            piezaSeleccionada->regresarPosicionInicial();
            qDebug() << "ERROR: Pieza distractora dentro del maletín. -5s";
        } else {
            // Mover random por la mesa no penaliza
            piezaSeleccionada->regresarPosicionInicial();
            qDebug() << "Pieza distractora soltada fuera del maletín. Sin penalización.";
        }
    }

    piezaSeleccionada = nullptr;

    // Cerrar maletín cuando las 3 piezas correctas estén bien colocadas
    if (maletin && maletin->estaCompleto() && !maletin->cerrado) {
        maletin->cerrar();
        qDebug() << "¡Maletín cerrado! Nivel completado.";
    }
}

void Nivel2::alternarLibro() {
    mostrarLibro = !mostrarLibro;
}

void Nivel2::dibujarLibro(QPainter* painter)
{
    painter->save();

    // 1) Oscurecer el juego detrás
    painter->fillRect(0, 0, 1600, 900, QColor(0, 0, 0, 200));

    if (!libroSprite.isNull()) {
        // 2) Dibujar la carta/libro centrada, usando la imagen como fondo

        // Tamaño destino en pantalla (ajusta si quieres más grande/pequeño)
        const int anchoDestino = 900;
        const int altoDestino  = 600;

        QRectF cartaRect(
            (1600 - anchoDestino) / 2.0,   // centrado en X
            (900  - altoDestino)  / 2.0,   // centrado en Y
            anchoDestino,
            altoDestino
            );

        // Imagen de la carta como fondo
        painter->drawPixmap(cartaRect.toRect(), libroSprite);

        // 3) Texto encima de la carta (si quieres seguir usando el texto)
        painter->setPen(Qt::black);
        painter->setFont(QFont("Georgia", 15));

        const QString historia =
            QStringLiteral(
                "El maletín yace abierto. Un susurro del destino.\n"
                "Piezas dispersas, algunas vitales, otras... meros fantasmas.\n\n"
                "Recuerda el orden de la creación:\n"
                "Primero, la chispa que enciende la voluntad.\n"
                "Luego, la fuerza que da forma al propósito.\n"
                "Finalmente, el tic-tac que sentencia el momento.\n\n"
                "Cada error en el ensamblaje, cada pieza ajena que ose tocar el corazón del artefacto,\n"
                "te robará un aliento precioso.\n\n"
                "La mesa es tu lienzo, el maletín tu destino.\n"
                "Pulsa L para que el secreto vuelva a la sombra."
                );

        // Margen interior dentro de la carta donde se dibuja el texto
        QRectF textoRect = cartaRect.adjusted(80.0, 120.0, -80.0, -80.0);
        painter->drawText(textoRect,
                          Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap,
                          historia);
    } else {
        // Fallback si la imagen no se cargó: usa el rectángulo anterior
        QColor coverColor(70, 90, 70);
        QColor pageColor (230, 225, 210);
        QColor frameColor(0, 0, 0);

        QRectF libroRect(260.0, 120.0, 1080.0, 640.0);

        painter->setBrush(coverColor);
        painter->setPen(QPen(frameColor, 5));
        painter->drawRoundedRect(libroRect, 18, 18);

        QRectF pageRect = libroRect.adjusted(18, 18, -18, -18);
        painter->setBrush(pageColor);
        painter->setPen(QPen(frameColor, 2));
        painter->drawRoundedRect(pageRect, 12, 12);

        painter->setPen(Qt::black);
        painter->setFont(QFont("Georgia", 26, QFont::Bold));
        painter->drawText(pageRect.adjusted(0.0, 10.0, 0.0, -540.0),
                          Qt::AlignTop | Qt::AlignHCenter,
                          QStringLiteral("NIVEL 2: ENSAMBLAJE DEL ARTEFACTO"));
    }

    painter->restore();
}
