QT       += core gui network widgets
CONFIG   += c++17
TARGET    = mini-redis-ui
TEMPLATE  = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    consolepanel.cpp \
    keybrowserpanel.cpp \
    infopanel.cpp \
    monitorpanel.cpp \
    cheatsheetpanel.cpp \
    redisclient.cpp \
    resparser.cpp

HEADERS += \
    mainwindow.h \
    consolepanel.h \
    keybrowserpanel.h \
    infopanel.h \
    monitorpanel.h \
    cheatsheetpanel.h \
    redisclient.h \
    resparser.h

FORMS += \
    mainwindow.ui \
    consolepanel.ui \
    keybrowserpanel.ui \
    infopanel.ui \
    monitorpanel.ui \
    cheatsheetpanel.ui

RESOURCES += resources.qrc

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
