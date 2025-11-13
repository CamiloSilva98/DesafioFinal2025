#ifndef NIVEL_H
#define NIVEL_H

#include <QTimer>
#include <QMainWindow>

class QPainter;

class Nivel {
protected:
    int numeroNivel;
    bool completado;
    QTimer* temporizador;

public:
    Nivel(int numero);
    virtual ~Nivel();

    virtual void inicializar() = 0;
    virtual void actualizar(float dt) = 0;
    virtual void renderizar(QPainter* painter) = 0;
    virtual bool verificarCondicionVictoria() = 0;
    virtual bool verificarCondicionDerrota() = 0;

    bool estaCompletado() const { return completado; }
    int getNumeroNivel() const { return numeroNivel; }
};

#endif
