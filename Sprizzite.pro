#-------------------------------------------------
#
# Project created by QtCreator 2016-10-19T15:47:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Sprizzite
TEMPLATE = app


SOURCES += main.cpp\
        sprizzite.cpp \
    model.cpp \
    filedialog.cpp \
    pixelcanvas.cpp \
    sprite.cpp \
    viewport.cpp

HEADERS  += sprizzite.h \
    model.h \
    filedialog.h \
    pixelcanvas.h \
    sprite.h \
    viewport.h \
    gif.h


FORMS    += \
    filedialog.ui \
    sprizzite.ui

RESOURCES += \
    buttons.qrc \
    buttons/previewbuttons.qrc

DISTFILES += \
    buttons/fastForward.jpg \
    buttons/rewind.jpg \
    buttons/pause.png \
    buttons/play.png
