#-------------------------------------------------
#
# Project created by QtCreator 2017-07-14T17:02:56
#
#-------------------------------------------------

QT       += core widgets opengl webenginewidgets

TARGET = director_service
TEMPLATE = lib

DEFINES += DS_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += precompile_header
PRECOMPILED_HEADER  = ds_global.h

SOURCES += \
        ds.cpp \
    hglwidget.cpp \
    hmainwidget.cpp \
    hdsctx.cpp \
    tga.cpp \
    haudioplay.cpp \
    htitlebarwidget.cpp \
    htoolbarwidget.cpp \
    hrcloader.cpp \
    hmaintoolbar.cpp \
    hffmpeg.cpp

HEADERS += \
        ds.h \
        ds_global.h \ 
    hglwidget.h \
    hmainwidget.h \
    hdsctx.h \
    tga.h \
    haudioplay.h \
    htitlebarwidget.h \
    htoolbarwidget.h \
    hrcloader.h \
    hmaintoolbar.h \
    hffmpeg.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

INCLUDEPATH += ./ook/ \
               /usr/include/freetype2/

LIBS += -lGL \
        -lGLU \
        -lGLEW \
        -lfreetype \
        -lftgl \
        -L../lib/ubuntu12/x86_64 \
        -ltrans \
        -lportaudio

