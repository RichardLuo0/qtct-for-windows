include(../../qtct.pri)

TEMPLATE = lib
TARGET = qtct-style-hack$$SUFFIX
QT += widgets

CONFIG += plugin

target.path = $$PLUGINDIR/styles-hack
INSTALLS += target

QTCT_STYLE = ../qtct-style

INCLUDEPATH += ../ $$QTCT_STYLE/

HEADERS += \
    $$QTCT_STYLE/qtctproxystyle.h

SOURCES += \
    plugin.cpp \
    $$QTCT_STYLE/qtctproxystyle.cpp

OTHER_FILES += \
    qtct.json
