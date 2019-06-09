QT += core gui widgets network

QMAKE_CXXFLAGS = -std=c++11
QMAKE=LFLAGS = -std=c++11

QT += testlib

HEADERS += \
    map.h \
    robot.h \
    position.h \
    mapgui.h \
    tcpServerManager.h \
    message.h

SOURCES += \
    map.cpp \
    robot.cpp \
    position.cpp \
    mapgui.cpp \
    main.cpp \
    tcpServerManager.cpp \
    message.cpp
