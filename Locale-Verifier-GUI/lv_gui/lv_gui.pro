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
    QMAKE_POST_LINK += $$quote(cp $$PWD/locale-verifier $$OUT_PWD$$escape_expand(\n\t))
    QMAKE_POST_LINK += $$quote(cp $$PWD/dict.png $$OUT_PWD$$escape_expand(\n\t))
}
 win32 {
    QMAKE_POST_LINK += $$quote(cmd /c copy /y $$PWD\locale-verifier $$OUT_PWD$$escape_expand(\n\t))
    QMAKE_POST_LINK += $$quote(cmd /c copy /y $$PWD\dict.png $$OUT_PWD$$escape_expand(\n\t))
}
