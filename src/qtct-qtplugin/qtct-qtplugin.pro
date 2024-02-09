include(../../qtct.pri)

TEMPLATE = lib
TARGET = qtct$$SUFFIX
CONFIG += plugin

QT += gui-private

SOURCES += \
    main.cpp \
    qtctplatformtheme.cpp

!equals(DISABLE_WIDGETS,1) {
  QT += widgets
}

OTHER_FILES += qtct.json

INCLUDEPATH += ../

HEADERS += \
    qtctplatformtheme.h

target.path = $$PLUGINDIR/platformthemes
INSTALLS += target
