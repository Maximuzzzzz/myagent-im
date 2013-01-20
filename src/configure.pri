DEFINES += VERSION=\\\"0.5.0\\\"

unix {
    isEmpty(PREFIX):PREFIX = /usr/local
    BINDIR = $$PREFIX/bin
    DATADIR = $$PREFIX/share/myagent-im
    HICOLOR = $$PREFIX/share/icons/hicolor
    DEFINES += DATADIR=\\\"$$DATADIR\\\"
}

QMAKE_CXXFLAGS += -Werror=return-type
