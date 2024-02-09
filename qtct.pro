CONFIG += ordered

TEMPLATE = subdirs

SUBDIRS += src/qtct-common src/qtct-qtplugin src/qtct-style src/qtct-style-hack

equals(QT_MAJOR_VERSION, 6) {
  SUBDIRS += src/qtct
}

unix:exists($$[QT_INSTALL_BINS]/lrelease){
LRELEASE_EXECUTABLE = $$[QT_INSTALL_BINS]/lrelease
}

unix:exists($$[QT_INSTALL_BINS]/lrelease-qt6){
LRELEASE_EXECUTABLE = $$[QT_INSTALL_BINS]/lrelease-qt6
}


win32:exists($$[QT_INSTALL_BINS]/lrelease.exe){
LRELEASE_EXECUTABLE = $$[QT_INSTALL_BINS]/lrelease.exe
}

isEmpty(LRELEASE_EXECUTABLE){
error(Could not find lrelease executable)
}
else {
message(Found lrelease executable: $$LRELEASE_EXECUTABLE)
}

message(generating translations)
unix:system(find . -name *.ts | xargs $$LRELEASE_EXECUTABLE)
win32:system(for /r %B in (*.ts) do $$LRELEASE_EXECUTABLE %B)

include(qtct.pri)

qss.files = qss/*.qss
qss.path = $$DATADIR/qss

colors.files = colors/*.conf
colors.path = $$DATADIR/colors

others.files = AUTHORS COPYING ChangeLog README.md mklink.bat
others.path = $$PREFIX

INSTALLS += qss colors others

message (PREFIX=$$PREFIX)
message (BINDIR=$$BINDIR)
message (LIBDIR=$$LIBDIR)
message (PLUGINDIR=$$PLUGINDIR)
equals (DISABLE_WIDGETS,1):message ("Qt Widgets are disabled!")
