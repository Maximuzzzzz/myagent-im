clebsCheck(swfdec-qt4) {
    CLEBS_DEPENDENCIES *= swfdec-qt4
}

clebsDependency(swfdec-qt4) {
#    LIBS += -lswfdec-qt4-0.1 -L$$DESTDIR

    CLEBS *= chrpath
#    QT += gui

#    INCLUDEPATH += $$BASEDIR/src
#    DEPENDPATH += $$BASEDIR/src
}
