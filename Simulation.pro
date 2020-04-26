greaterThan(QT_MAJOR_VERSION, 4) {
    QT *= widgets
}
QT += sql

HEADERS += \
    vanne.h \
    reservoir.h \
    pompe.h \
    avion.h \
    mainwindow.h \
    fenetrepilote.h \
    identification.h \
    affichertexte.h

SOURCES += \
    vanne.cpp \
    reservoir.cpp \
    pompe.cpp \
    main.cpp \
    avion.cpp \
    mainwindow.cpp \
    fenetrepilote.cpp \
    identification.cpp \
    affichertexte.cpp

FORMS += \
    mainwindow.ui \
    fenetrepilote.ui \
    identification.ui \
    demandeMdp.ui \
    affichertexte.ui

RESOURCES += \
    images.qrc

