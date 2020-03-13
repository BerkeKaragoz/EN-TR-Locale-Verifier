TEMPLATE = app
TARGET = lv_gui

QT = core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES +=  main.cpp \
    dialog.cpp

HEADERS += \
    dialog.h

DISTFILES += \
    locale-verifier.o

FORMS += \
    dialog.ui
