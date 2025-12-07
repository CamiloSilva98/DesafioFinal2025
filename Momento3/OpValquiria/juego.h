#ifndef JUEGO_H
#define JUEGO_H

#include <QMainWindow>
#include <QTimer>

class Nivel;
class QPainter;

enum class Estado {
    MENU,
    JUGANDO,
    PAUSADO,
    VICTORIA,
    DERROTA
};

class Juego : public QMainWindow {
    Q_OBJECT

private:
    Nivel* nivelActual;
    int puntuacion;
    int vidas;
    Estado estadoJuego;
    QTimer* timer;
    int nivelActualNumero;

    // Métodos privados para dibujar
    void dibujarMenu(QPainter& painter);
    void dibujarHUD(QPainter& painter);
    void dibujarPausa(QPainter& painter);
    void dibujarVictoria(QPainter& painter);
    void dibujarDerrota(QPainter& painter);
    void cargarSiguienteNivel();

public:
    Juego(QWidget* parent = nullptr);
    ~Juego();

    void iniciar();
    void cambiarNivel(int nivel);
    void actualizarPuntuacion(int puntos);
    void pausar();
    void terminar();


protected:
    void keyPressEvent(QKeyEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event)override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
private slots:
    void actualizar();
};

#endif
