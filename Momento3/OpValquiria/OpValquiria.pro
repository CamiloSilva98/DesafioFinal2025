QT       += core gui\
            widgets multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    agenteinteligente.cpp \
    avion.cpp \
    cuadro.cpp \
    decoracion.cpp \
    entidad.cpp \
    guardia.cpp \
    juego.cpp \
    jugador.cpp \
    llave.cpp \
    main.cpp \
    mainwindow.cpp \
    misil.cpp \
    nivel.cpp \
    nivel1.cpp \
    nivel2.cpp \
    nivel3.cpp \
    maletin.cpp \
    pieza.cpp \
    objetointeractivo.cpp \
    obstaculo.cpp \
    planos.cpp \
    puerta.cpp\
    origin/main

HEADERS += \
    agenteinteligente.h \
    avion.h \
    cuadro.h \
    decoracion.h \
    entidad.h \
    guardia.h \
    juego.h \
    jugador.h \
    llave.h \
    mainwindow.h \
    misil.h \
    nivel.h \
    nivel1.h \
    nivel2.h \
    nivel3.h \
    maletin.h \
    pieza.h \
    objetointeractivo.h \
    obstaculo.h \
    planos.h \
    puerta.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc \
    sprites.qrc
