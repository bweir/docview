QT += gui widgets webkitwidgets

TEMPLATE = app
TARGET = spindoctor
DESTDIR = ../bin/

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
