QT += svg
TARGET = qxkb
TEMPLATE = app
SOURCES += src/qxkb.cpp src/xkeyboard.cpp src/xkbconf.cpp src/x11tools.cpp src/main.cpp src/datamodels.cpp
HEADERS += src/qxkb.h src/xkeyboard.h src/xkbconf.h src/x11tools.h src/datamodels.h src/defs.h
FORMS += ui/kswconfig.ui 
RESOURCES = qrc/icon.qrc
DESTDIR += bin
OBJECTS_DIR += build
MOC_DIR += build
QMAKE_INCDIR += /usr/include 
QMAKE_CLEAN += qxkb
CONFIG += warn_on debug
LIBS += -lxkbfile -lX11
TRANSLATIONS = language/qxkb_ru.ts

# version for C++ code too ************

VERSION = 0.4.3-RC1
DEFINES += "QXKB_VERSION=\\\"$$VERSION\\\""

# installs ****************************
target.path += /usr/bin/

translations.files += language/*.qm
translations.path += $$[QT_INSTALL_TRANSLATIONS] /usr/share/qt4/translations

theme.files += theme/*
theme.path += /usr/share/qxkb/theme

desktop.files += desktop/qxkb.desktop
desktop.path += /usr/share/applications

icon.files += desktop/qxkb.png
icon.path += /usr/share/pixmaps

INSTALLS += target translations theme desktop icon

