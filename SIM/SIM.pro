QT += core gui widgets network

QMAKE_CXXFLAGS = -std=c++11
QMAKE=LFLAGS = -std=c++11

QT += testlib

HEADERS += \
    map.h \
    robot.h \
    position.h \
    tcpServerManager.h \
    message.h \
    mathhelper.h \
    simgui.h \
    mapgui.h \
    messagemanager.h

SOURCES += \
    map.cpp \
    robot.cpp \
    position.cpp \
    main.cpp \
    tcpServerManager.cpp \
    message.cpp \
    mathhelper.cpp \
    simgui.cpp \
    mapgui.cpp \
    messagemanager.cpp
