## The famous qt5ct and qt6ct is on windows!

This tool works for both qt5 and qt6.

Remember you can **not** mix different environment together (by which I mean MSVC or mingw). The apps have to be compiled with the same compiler that compiles qtct.

You can find several new styles in [qtstyleplugins](https://code.qt.io/cgit/qt/qtstyleplugins.git/) (You need to compile with the same compiler)

You can put icons theme in `~\.icons`

For kvantum, you can use [kvantum-for-windows](https://github.com/RichardLuo0/kvantum-for-windows)

# Installation
* Download the latest release and extract in a folder.
* Set env variable `QT_QPA_PLATFORMTHEME=qtct`.
* * Set env variable `QT_STYLE_OVERRIDE=qtct-style`.
* ~~Set env variable `QT_PLUGIN_PATH=.;<path to your qtct>;`~~. The best way is just go to each app path and create a soft link from `<qtct path>/styles` to `<your app path>\styles` and for `platformthemes` as well
* **Notice: if selected style is not applied to certain apps, its most probably that the app hardcoded its own style. You will have to compile from source, download the source, set the content of `src\qtct-style-hack\qtct.json` to the style that the application is using, compile and replace the original style with `styles/qtct-style-hack.dll` in the app directory styles. (My release version contains hack for breeze theme, which is default to kde apps)**

# Files and Directories

* `qtct.exe` - Qt configuration tool
* `platformthemes/qtct.dll` - qtct platform plugin
* `styles/qtct-style.dll` - qtct proxy style plugin
* `%LocalAppData%/qtct/qtct.conf` - configuration file
* `%LocalAppData%/qtct/qss`, `%LocalAppData%/qtct/colors`, `share/qss`, `share/colors` style sheets and colors
* `mklink.bat` drag app folder on it to auto create soft link into app folder.

---

qt6ct - Qt6 Configuration Tool

This program allows users to configure Qt6 settings (theme, font, icons, etc.)
under DE/WM without Qt integration.

Official home page: https://github.com/trialuser02/qt6ct

Requirements:

- GNU Linux or FreeBSD
- qtbase >= 6.0.0
- qtsvg >= 6.0.0 (Runtime dependency for the SVG icon support)
- qttools >= 6.0.0 (For build only)


Installation:

- Arch package
  https://archlinux.org/packages/?q=qt6ct

- Source Code (qmake)
  qmake PREFIX=<your installation path>
  make
  sudo make install

- Source Code (cmake)
  cmake -DCMAKE_INSTALL_PREFIX=<your installation path>
  make
  sudo make install

Add line 'export QT_QPA_PLATFORMTHEME=qt6ct' to ~/.profile and re-login.
Alternatively, create the file /etc/X11/Xsession.d/100-qt6ct with
the following line:

export QT_QPA_PLATFORMTHEME=qt6ct

(or qt5ct for compatibility with Qt5 Configuration Tool)

Now restart X11 server to take the changes effect.

Extra build options (for advanced users only):
qmake DISABLE_WIDGETS=1 - compiles platform plugin without QtWidgets (useful for QML applications only)
qmake PLUGINDIR=<custom path> - changes the default installation path of the plugins (libqt6ct.so and libqt6ct-style.so)

Wayland:

If you are running Qt application under Wayland, you should export the following variables:

export QT_QPA_PLATFORM=wayland
export QT_QPA_PLATFORMTHEME=qt6ct

Debug Messages:

By default, debug messages from platform plugin are disabled. To enable them, you should
export the following environment variable:

export QT_LOGGING_RULES="qt6ct.debug=true"

Another way is to change qtlogging.ini file. See Qt documentation for details:
https://doc.qt.io/qt-6/qloggingcategory.html (paragraph "Configuring Categories")

Files and Directories:

qt6ct - Qt6 configuration tool
libqt6ct.so - qt6ct platform plugin
libqt6ct-style.so - qt6ct proxy style plugin
libqt6ct-common.so - qt6ct shared library
~/.config/qt6ct/qt6ct.conf - configuration file
~/.config/qt6ct/qss, ~/.local/share/qt6ct/qss, /usr/share/qt6ct/qss, /usr/local/share/qt6ct/qss - style sheets
~/.config/qt6ct/colors,~/.local/share/qt6ct/colors, /usr/share/qt6ct/colors,
/usr/local/share/qt6ct/colors - color schemes
/etc/xdg/qt6ct/qt6ct.conf - default configuration file

Translation:

Use Transifex service: https://www.transifex.com/projects/p/qt6ct/
