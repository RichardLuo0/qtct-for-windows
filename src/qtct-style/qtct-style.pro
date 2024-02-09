include(../../qtct.pri)

TEMPLATE = lib
TARGET = qtct-style$$SUFFIX
QT += widgets

CONFIG += plugin

target.path = $$PLUGINDIR/styles
INSTALLS += target

INCLUDEPATH += ../

HEADERS += \
    qtctproxystyle.h

SOURCES += \
    plugin.cpp \
    qtctproxystyle.cpp

OTHER_FILES += \
    qtct.json
