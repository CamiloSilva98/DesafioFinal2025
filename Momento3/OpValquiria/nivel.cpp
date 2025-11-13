#include "nivel.h"

Nivel::Nivel(int numero)
    : numeroNivel(numero),
    completado(false),
    temporizador(nullptr)
{
}

Nivel::~Nivel() {
    if (temporizador) {
        delete temporizador;
    }
}
