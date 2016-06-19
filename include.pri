INCLUDEPATH += $$PWD/include/
LIBS        += -L$$PWD/lib/ -lspindoctor

win32-msvc* {
	PRE_TARGETDEPS += $$PWD/lib/spindoctor.lib
} else {
	PRE_TARGETDEPS += $$PWD/lib/libspindoctor.a
}
