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

equals(QT_MAJOR_VERSION, 5) {
  windowsvista.extra = cp $$[QT_INSTALL_PLUGINS]/styles/qwindowsvistastyle.dll $$target.path/qwindowsvistastyle-qt5.dll
  windowsvista.path = $$target.path
  INSTALLS += windowsvista
}
