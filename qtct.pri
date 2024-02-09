#Some conf to redirect intermediate stuff in separate dirs
UI_DIR=./.build/ui/
MOC_DIR=./.build/moc/
OBJECTS_DIR=./.build/obj
RCC_DIR=./.build/rcc

QMAKE_DISTCLEAN += -r .build
QMAKE_DISTCLEAN += translations/*.qm

CONFIG += hide_symbols c++17 ordered
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000 QT_DEPRECATED_WARNINGS_SINCE=0x060200
DEFINES += QT_NO_CAST_FROM_BYTEARRAY QT_STRICT_ITERATORS QT_NO_FOREACH
QMAKE_DISTCLEAN += -r .build

#*-g++ {
#  QMAKE_CXXFLAGS += -Werror=suggest-override
#  QMAKE_CXXFLAGS += -Wzero-as-null-pointer-constant
#}

INCLUDEPATH += ../qtct-common
CONFIG(debug, debug|release) {
  QMAKE_LIBDIR += ../qtct-common/debug
} else {
  QMAKE_LIBDIR += ../qtct-common/release
}
LIBS += -lwin-qtct-common

MAJ = 0
MIN = 1
DEFINES += QTCT_VERSION_MAJOR=$$MAJ
DEFINES += QTCT_VERSION_MINOR=$$MIN

#Install paths
unix {
  isEmpty(PREFIX): PREFIX = /usr
  isEmpty(PLUGINDIR): PLUGINDIR = $$[QT_INSTALL_PLUGINS]
  isEmpty(LIBDIR): LIBDIR=$$[QT_INSTALL_LIBS]

  BINDIR = $$PREFIX/bin
  DATADIR = $$PREFIX/share

  DEFINES += QTCT_DATADIR=\\\"$$DATADIR\\\"
}

win32 {
  isEmpty(PREFIX): PREFIX = "C:/qtct"
  isEmpty(PLUGINDIR): PLUGINDIR = $$PREFIX
  isEmpty(LIBDIR): LIBDIR=$$PREFIX

  BINDIR = $$PREFIX
  DATADIR = $$PREFIX/share

  equals(QT_MAJOR_VERSION, 6) {
    SUFFIX=
  } else {
    SUFFIX=-qt5
  }
}
