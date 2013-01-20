isEmpty(APP_VERSION) {
    DEFINES += VERSION=\\\"0.4.7-devel\\\"
} else {
    DEFINES += VERSION=\\\"$${APP_VERSION}\\\"
}

unix {
    isEmpty(PREFIX):PREFIX = /usr/local
    BINDIR = $$PREFIX/bin
    DATADIR = $$PREFIX/share/myagent-im
    HICOLOR = $$PREFIX/share/icons/hicolor
    DEFINES += DATADIR=\\\"$$DATADIR\\\"
}

QMAKE_CXXFLAGS += -Werror=return-type

*g++* {
    QMAKE_CXXFLAGS_DEBUG += \
        -Wextra \
        -Wlogical-op \
        -Wold-style-cast \
        -Woverloaded-virtual \
        -Winit-self \
        -Wunreachable-code \
        -Wctor-dtor-privacy \
        -Wnon-virtual-dtor \
        -Wconversion \
        -pedantic

    QMAKE_CXXFLAGS_DEBUG += -isystem /usr/include/qt4/QtCore
    QMAKE_CXXFLAGS_DEBUG += -isystem /usr/include/qt4/QtGui
    QMAKE_CXXFLAGS_DEBUG += -isystem /usr/include/qt4/QtDBus
}
