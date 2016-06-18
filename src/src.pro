QT += gui widgets

TEMPLATE = app
TARGET = spindoctor
DESTDIR = ../bin/

HEADERS += \
    spindoctor.h \

SOURCES += \
    spindoctor.cpp \
    main.cpp \
    $$files($$PWD/hoedown/src/*)

INCLUDEPATH += $$PWD/hoedown/src/

RESOURCES += \
    ../templates/templates.qrc \
