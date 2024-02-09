/*
 * Copyright (c) 2020-2024, Ilya Kotov <forkotov02@ya.ru>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <QFont>
#include <QGuiApplication>
#include <QIcon>
#include <QMimeDatabase>
#include <QPalette>
#include <QRegularExpression>
#include <QScreen>
#include <QSettings>
#include <QTimer>
#include <QVariant>
#ifdef QT_WIDGETS_LIB
#include <QApplication>
#include <QStyle>
#include <QStyleFactory>
#include <QWidget>
#endif
#include <private/qiconloader_p.h>
#include <qpa/qplatformthemefactory_p.h>

#include <QFile>
#include <QFileSystemWatcher>
#include <QStringList>

#include "qtct.h"
#include "qtctplatformtheme.h"

Q_LOGGING_CATEGORY(lqtct, "qtct", QtWarningMsg)

// QT_QPA_PLATFORMTHEME=qtct

QtCTPlatformTheme::QtCTPlatformTheme() {
  winTheme.reset(
      QGuiApplicationPrivate::platform_integration->createPlatformTheme(
          "windows"));

  QtCT::initConfig();
  if (QGuiApplication::desktopSettingsAware()) {
    readSettings();
    QMetaObject::invokeMethod(this, "applySettings", Qt::QueuedConnection);
#ifdef QT_WIDGETS_LIB
    QMetaObject::invokeMethod(this, "createFSWatcher", Qt::QueuedConnection);
#endif
    if (m_fontOverride) QGuiApplication::setFont(m_generalFont);
  }
  qCDebug(lqtct) << "using qtct plugin";
#ifdef QT_WIDGETS_LIB
  if (!QStyleFactory::keys().contains("qtct-style"))
    qCCritical(lqtct) << "unable to find qtct proxy style";
#endif
}

QtCTPlatformTheme::~QtCTPlatformTheme() {}

QPlatformMenuItem *QtCTPlatformTheme::createPlatformMenuItem() const {
  return winTheme->createPlatformMenuItem();
}

QPlatformMenu *QtCTPlatformTheme::createPlatformMenu() const {
  return winTheme->createPlatformMenu();
}

QPlatformMenuBar *QtCTPlatformTheme::createPlatformMenuBar() const {
  return winTheme->createPlatformMenuBar();
}

void QtCTPlatformTheme::showPlatformMenuBar() {
  return winTheme->showPlatformMenuBar();
}

bool QtCTPlatformTheme::usePlatformNativeDialog(DialogType type) const {
  return m_theme ? m_theme->usePlatformNativeDialog(type)
                 : winTheme->usePlatformNativeDialog(type);
}

QPlatformDialogHelper *QtCTPlatformTheme::createPlatformDialogHelper(
    DialogType type) const {
  return m_theme ? m_theme->createPlatformDialogHelper(type)
                 : winTheme->createPlatformDialogHelper(type);
}

QPlatformSystemTrayIcon *QtCTPlatformTheme::createPlatformSystemTrayIcon()
    const {
  return winTheme->createPlatformSystemTrayIcon();
}

const QPalette *QtCTPlatformTheme::palette(QPlatformTheme::Palette type) const {
  return (m_usePalette && !m_palette.isValid())
             ? new QPalette(m_palette.value<QPalette>())
             : winTheme->palette(type);
}

const QFont *QtCTPlatformTheme::font(QPlatformTheme::Font type) const {
  if (m_fontOverride) {
    if (type == QPlatformTheme::FixedFont) return &m_fixedFont;
    return &m_generalFont;
  } else {
    return winTheme->font(type);
  }
}

QVariant QtCTPlatformTheme::themeHint(QPlatformTheme::ThemeHint hint) const {
  if (m_isIgnored) return winTheme->themeHint(hint);

  switch (hint) {
    case QPlatformTheme::CursorFlashTime:
      return m_cursorFlashTime;
    case MouseDoubleClickInterval:
      return m_doubleClickInterval;
    case QPlatformTheme::ToolButtonStyle:
      return m_toolButtonStyle;
    case QPlatformTheme::SystemIconThemeName:
      return m_iconTheme;
    case QPlatformTheme::StyleNames:
      return QStringList() << "qtct-style";
    case QPlatformTheme::IconThemeSearchPaths:
      return QtCT::iconPaths();
    case QPlatformTheme::DialogButtonBoxLayout:
      return m_buttonBoxLayout;
    case QPlatformTheme::KeyboardScheme:
      return m_keyboardScheme;
    case QPlatformTheme::UiEffects:
      return m_uiEffects;
    case QPlatformTheme::WheelScrollLines:
      return m_wheelScrollLines;
    case QPlatformTheme::ShowShortcutsInContextMenus:
      return m_showShortcutsInContextMenus;
    default:
      return winTheme->themeHint(hint);
  }
}

QIcon QtCTPlatformTheme::fileIcon(
    const QFileInfo &fileInfo, QPlatformTheme::IconOptions iconOptions) const {
  if ((iconOptions & DontUseCustomDirectoryIcons) && fileInfo.isDir())
    return QIcon::fromTheme(QLatin1String("inode-directory"));

  QMimeDatabase db;
  QMimeType type = db.mimeTypeForFile(fileInfo);
  return QIcon::fromTheme(type.iconName());
}

QPixmap QtCTPlatformTheme::standardPixmap(StandardPixmap sp,
                                          const QSizeF &size) const {
  return winTheme->standardPixmap(sp, size);
};

QIconEngine *QtCTPlatformTheme::createIconEngine(
    const QString &iconName) const {
  return winTheme->createIconEngine(iconName);
}

void QtCTPlatformTheme::applySettings() {
  if (!QGuiApplication::desktopSettingsAware() || m_isIgnored) {
    m_usePalette = false;
    m_update = true;
    return;
  }

  if (!m_update) {
    // do not override application palette
    if (QCoreApplication::testAttribute(Qt::AA_SetPalette)) {
      m_usePalette = false;
      qCDebug(lqtct) << "palette support is disabled";
    }
  }

  if (m_fontOverride) QGuiApplication::setFont(m_generalFont);  // apply font

#ifdef QT_WIDGETS_LIB
  if (hasWidgets()) {
    if (m_fontOverride) qApp->setFont(m_generalFont);

    // Qt 5.6 or higher should be use themeHint function on application startup.
    // So, there is no need to call this function first time.
    if (m_update) {
      qApp->setWheelScrollLines(m_wheelScrollLines);
      QtCT::reloadStyleInstanceSettings();
    }

    if (m_update && m_usePalette && !m_palette.isValid())
      qApp->setPalette(m_palette.value<QPalette>());

    if (m_userStyleSheet != m_prevStyleSheet) {
      // prepend our stylesheet to that of the application
      // (first removing any previous stylesheet we have set)
      QString appStyleSheet = qApp->styleSheet();
      int prevIndex = appStyleSheet.indexOf(m_prevStyleSheet);
      if (prevIndex >= 0) {
        appStyleSheet.remove(prevIndex, m_prevStyleSheet.size());
        qApp->setStyleSheet(m_userStyleSheet + appStyleSheet);
      } else {
        qCDebug(lqtct) << "custom style sheet is disabled";
      }
      m_prevStyleSheet = m_userStyleSheet;
    }
  }
#endif

  if (m_update) QIconLoader::instance()->updateSystemTheme();  // apply icons

#ifdef QT_WIDGETS_LIB
  if (hasWidgets() && m_update) {
    for (QWidget *w : qApp->allWidgets()) {
      QEvent e(QEvent::ThemeChange);
      QApplication::sendEvent(w, &e);
      if (m_usePalette && !m_palette.isValid())
        w->setPalette(m_palette.value<QPalette>());
    }
  }
#endif

  m_update = true;
}

#ifdef QT_WIDGETS_LIB
void QtCTPlatformTheme::createFSWatcher() {
  QFileSystemWatcher *watcher = new QFileSystemWatcher(this);
  watcher->addPath(QtCT::configPath());

  QTimer *timer = new QTimer(this);
  timer->setSingleShot(true);
  timer->setInterval(3000);
  connect(watcher, SIGNAL(directoryChanged(QString)), timer, SLOT(start()));
  connect(timer, SIGNAL(timeout()), SLOT(updateSettings()));
}

void QtCTPlatformTheme::updateSettings() {
  qCDebug(lqtct) << "updating settings..";
  readSettings();
  applySettings();
}
#endif

QHash<QString, int> effectsMap{
    {"General", QPlatformTheme::GeneralUiEffect},
    {"AnimateMenu", QPlatformTheme::AnimateMenuUiEffect},
    {"FadeMenu", QPlatformTheme::FadeMenuUiEffect},
    {"AnimateCombo", QPlatformTheme::AnimateComboUiEffect},
    {"AnimateTooltip", QPlatformTheme::AnimateTooltipUiEffect},
    {"FadeTooltip", QPlatformTheme::FadeTooltipUiEffect},
    {"AnimateToolBox", QPlatformTheme::AnimateToolBoxUiEffect}};

void QtCTPlatformTheme::readSettings() {
  QSettings settings(QtCT::configFile(), QSettings::IniFormat);
  QtCT::qt5QSettingsCompatible(settings);

  settings.beginGroup("Appearance");
  m_style = settings.value("style", "Fusion").toString();
  QString schemePath = settings.value("color_scheme_path").toString();
  if (!schemePath.isEmpty() &&
      settings.value("custom_palette", false).toBool()) {
    schemePath = QtCT::resolvePath(schemePath);
    m_palette = QtCT::loadColorScheme(schemePath);
  } else {
    m_palette.clear();
  }
  m_iconTheme = settings.value("icon_theme").toString();
  // load dialogs
  if (!m_update) {
    // do not mix gtk2 style and gtk3 dialogs
    QStringList keys = QPlatformThemeFactory::keys();
    QString dialogs = settings.value("standard_dialogs", "default").toString();

    if (m_style.endsWith("gtk2") && dialogs == QLatin1String("gtk3"))
      dialogs = QLatin1String("gtk2");
    if (keys.contains(dialogs))
      m_theme.reset(QPlatformThemeFactory::create(dialogs));
  }

  settings.endGroup();

  settings.beginGroup("Fonts");
  m_fontOverride = settings.value("font_override", false).toBool();
  m_generalFont = QGuiApplication::font();
  m_fixedFont = QGuiApplication::font();
  m_generalFont = QtCT::qt5FontCompatible(
      settings.value("general", QGuiApplication::font()).toString());
  m_fixedFont = QtCT::qt5FontCompatible(
      settings.value("fixed", QGuiApplication::font()).toString());
  settings.endGroup();

  settings.beginGroup("Interface");
  m_doubleClickInterval =
      winTheme->themeHint(QPlatformTheme::MouseDoubleClickInterval).toInt();
  m_doubleClickInterval =
      settings.value("double_click_interval", m_doubleClickInterval).toInt();
  m_cursorFlashTime =
      winTheme->themeHint(QPlatformTheme::CursorFlashTime).toInt();
  m_cursorFlashTime =
      settings.value("cursor_flash_time", m_cursorFlashTime).toInt();
  m_showShortcutsInContextMenus =
      settings.value("show_shortcuts_in_context_menus", true).toBool();
  m_buttonBoxLayout =
      winTheme->themeHint(QPlatformTheme::DialogButtonBoxLayout).toInt();
  m_buttonBoxLayout =
      settings.value("buttonbox_layout", m_buttonBoxLayout).toInt();
  m_keyboardScheme =
      winTheme->themeHint(QPlatformTheme::KeyboardScheme).toInt();
  m_keyboardScheme =
      settings.value("keyboard_scheme", m_keyboardScheme).toInt();
  QCoreApplication::setAttribute(
      Qt::AA_DontShowIconsInMenus,
      !settings.value("menus_have_icons", true).toBool());
  m_toolButtonStyle =
      settings.value("toolbutton_style", Qt::ToolButtonFollowStyle).toInt();
  m_wheelScrollLines = settings.value("wheel_scroll_lines", 3).toInt();

  // load effects
  m_uiEffects = winTheme->themeHint(QPlatformTheme::UiEffects).toInt();
  if (settings.childKeys().contains("gui_effects")) {
    QStringList effectList = settings.value("gui_effects").toStringList();
    m_uiEffects = 0;
    for (auto &effect : effectList) {
      m_uiEffects |= effectsMap[effect];
    }
  }

  // load style sheets
#ifdef QT_WIDGETS_LIB
  QStringList qssPaths = settings.value("stylesheets").toStringList();
  m_userStyleSheet = loadStyleSheets(qssPaths);
#endif
  settings.endGroup();

  // load troubleshooting
  if (!m_update) {
    settings.beginGroup("Troubleshooting");
    m_isIgnored = settings.value("ignored_applications")
                      .toStringList()
                      .contains(QCoreApplication::applicationFilePath());
    int forceRasterWidgets =
        settings.value("force_raster_widgets", Qt::PartiallyChecked).toInt();
    if (!m_isIgnored && forceRasterWidgets == Qt::Checked)
      QCoreApplication::setAttribute(Qt::AA_ForceRasterWidgets, true);
    else if (!m_isIgnored && forceRasterWidgets == Qt::Unchecked)
      QCoreApplication::setAttribute(Qt::AA_ForceRasterWidgets, false);
    if (m_isIgnored) m_usePalette = false;
    settings.endGroup();
  }
}

#ifdef QT_WIDGETS_LIB
bool QtCTPlatformTheme::hasWidgets() {
  return qobject_cast<QApplication *>(qApp) != nullptr;
}
#endif

QString QtCTPlatformTheme::loadStyleSheets(const QStringList &paths) {
  QString content;
  for (const QString &path : qAsConst(paths)) {
    if (!QFile::exists(path)) continue;

    QFile file(path);
    file.open(QIODevice::ReadOnly);
    content.append(QString::fromUtf8(file.readAll()));
    if (!content.endsWith(QChar::LineFeed)) content.append(QChar::LineFeed);
  }
  static const QRegularExpression regExp("//.*\n");
  content.replace(regExp, "\n");
  return content;
}
