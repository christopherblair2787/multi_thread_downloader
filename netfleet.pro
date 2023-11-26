#check Qt version
QT_VERSION = $$[QT_VERSION]
QT_VERSION = $$split(QT_VERSION, ".")
QT_VER_MAJ = $$member(QT_VERSION, 0)
QT_VER_MIN = $$member(QT_VERSION, 1)

lessThan(QT_VER_MAJ, 4) | lessThan(QT_VER_MIN, 5) {
   error(NetFleet requires Qt 4.5 or newer but Qt $$[QT_VERSION] was detected.)
}

TEMPLATE = app
QT = gui \
    core \
    network
CONFIG += qt \
    warn_on \
    release
DESTDIR = bin
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
HEADERS = src/mainwindow.h \
    src/newtaskdialog.h \
    src/preferencesdialog.h \
    src/taskGraphPainterWidget.h \
    src/tools.h \
    src/speedGraphWidget.h \
    src/modifytaskdialog.h \
    src/downloadthread.h \
    src/httpdownload.h \
    src/ftpdownload.h \
    src/deletetaskdialog.h \
    src/smallwindow.h \
    src/newproxydialog.h \
    src/editproxydialog.h
SOURCES = src/main.cpp \
    src/mainwindow.cpp \
    src/newtaskdialog.cpp \
    src/preferencesdialog.cpp \
    src/taskGraphPainterWidget.cpp \
    src/tools.cpp \
    src/speedGraphWidget.cpp \
    src/modifytaskdialog.cpp \
    src/downloadthread.cpp \
    src/httpdownload.cpp \
    src/ftpdownload.cpp \
    src/deletetaskdialog.cpp \
    src/smallwindow.cpp \
    src/newproxydialog.cpp \
    src/editproxydialog.cpp
RESOURCES += netfleet.qrc
RC_FILE = icon.rc
FORMS += ui/newtask.ui \
    ui/preferences.ui \
    ui/delete.ui \
    ui/about.ui \
    ui/newproxy.ui
TRANSLATIONS += translations/netFleet_zh_CN.ts \
    translations/netFleet_zh_TW.ts \
    translations/netFleet_en_US.ts \
    translations/netFleet_de_DE.ts
unix { 
    target.path = $$/usr/bin
    icon.files = images/netfleet.xpm
    icon.path = /usr/share/pixmaps
    desktop.files = netfleet.desktop
    desktop.path = /usr/share/applications
    INSTALLS += target \
        icon \
        desktop
}
