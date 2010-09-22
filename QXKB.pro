# -------------------------------------------------
# Project created by QtCreator 2009-03-09T22:48:01
# -------------------------------------------------
QT += svg
TARGET = qxkb
TEMPLATE = app
SOURCES += src/qxkb.cpp \
    src/xkeyboard.cpp \
    src/xkbconf.cpp \
    src/x11tools.cpp \
    src/main.cpp \
    src/datamodels.cpp \
    src/about.cpp
HEADERS += src/qxkb.h \
    src/xkeyboard.h \
    src/xkbconf.h \
    src/x11tools.h \
    src/datamodels.h \
    src/defs.h \
    src/about.h
FORMS += ui/kswconfig.ui \
    ui/about.ui
RESOURCES = qrc/icon.qrc
DESTDIR += bin
OBJECTS_DIR += build
MOC_DIR += build
QMAKE_INCDIR += /usr/include \
    /usr/include/QxtGui \
    /usr/include/QxtCore
QMAKE_CLEAN += qxkb
CONFIG += qxt \
    warn_on \
    release
QXT += core \
    gui
//DEFINES = QT_NO_DEBUG_OUTPUT
LIBS += -lxkbfile \
    -lQxtCore \
    -lQxtGui
TRANSLATIONS = language/qxkb_ru.ts
VERSION = 0.4.3-pre
target.path += /usr/bin/
translations.files += language/*.qm
translations.path += $$[QT_INSTALL_TRANSLATIONS] \
    // \
    /usr/share/qt4/translations
theme.files += theme/*
theme.path += /usr/share/qxkb/theme
desktop.files += desktop/qxkb.desktop
desktop.path += /usr/share/applications
icon.files += desktop/qxkb.png
icon.path += /usr/share/pixmaps
INSTALLS += target \
    translations \
    theme \
    desktop \
    icon
