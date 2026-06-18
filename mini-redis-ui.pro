QT       += core gui network widgets
CONFIG   += c++17
TARGET    = mini-redis-ui
TEMPLATE  = app

INCLUDEPATH += include

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/consolepanel.cpp \
    src/keybrowserpanel.cpp \
    src/infopanel.cpp \
    src/monitorpanel.cpp \
    src/cheatsheetpanel.cpp \
    src/redisclient.cpp \
    src/resparser.cpp

HEADERS += \
    include/mainwindow.h \
    include/consolepanel.h \
    include/keybrowserpanel.h \
    include/infopanel.h \
    include/monitorpanel.h \
    include/cheatsheetpanel.h \
    include/redisclient.h \
    include/resparser.h

FORMS += \
    forms/mainwindow.ui \
    forms/consolepanel.ui \
    forms/keybrowserpanel.ui \
    forms/infopanel.ui \
    forms/monitorpanel.ui \
    forms/cheatsheetpanel.ui

RESOURCES += resources.qrc

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
