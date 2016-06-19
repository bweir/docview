QT += gui widgets webkitwidgets

CONFIG -= debug_and_release app_bundle

TEMPLATE = app
TARGET = spindoctor
DESTDIR = ../lib/

HEADERS += \
    spindoctor.h \
    markdown.h \

SOURCES += \
    spindoctor.cpp \
    markdown.cpp \
    main.cpp \
    $$files($$PWD/hoedown/src/*)

INCLUDEPATH += $$PWD/hoedown/src/

RESOURCES += \
    ../templates/templates.qrc \
