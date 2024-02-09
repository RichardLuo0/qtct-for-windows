include(../../qtct.pri)

TEMPLATE = lib
CONFIG += static
TARGET = win-qtct-common
LIBS -= -lwin-qtct-common

# Input

DEFINES += QTCT_LIBRARY

HEADERS += \
    qtct.h

SOURCES += \
    qtct.cpp
