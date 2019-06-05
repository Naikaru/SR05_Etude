QT += core gui widgets

QMAKE_CXXFLAGS = -std=c++11
QMAKE=LFLAGS = -std=c++11

QT += testlib

HEADERS += \
    map.h \
    robot.h \
    messagemanager.h \
    position.h \
    mathhelper.h \
    simgui.h \
    mapgui.h

SOURCES += \
    map.cpp \
    robot.cpp \
    messagemanager.cpp \
    position.cpp \
    main.cpp \
    mathhelper.cpp \
    simgui.cpp \
    mapgui.cpp
