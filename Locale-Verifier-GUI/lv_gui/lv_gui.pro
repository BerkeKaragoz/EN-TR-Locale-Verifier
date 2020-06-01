TEMPLATE = app
TARGET = lv_gui

QT = core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES +=  main.cpp \
    dialog.cpp

HEADERS += \
    dialog.h

FORMS += \
    dialog.ui

DISTFILES += \
    dict.png \
    locale-verifier

unix{
    QMAKE_POST_LINK += $$quote(cp /usr/bin/locale-verifier )
    QMAKE_POST_LINK += $$quote(cp /usr/share/locale-verifier/dict.png )
}
