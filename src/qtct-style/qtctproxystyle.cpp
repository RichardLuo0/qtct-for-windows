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

#include "qtctproxystyle.h"

#include <Windows.h>
#include <dwmapi.h>

#include <QApplication>
#include <QDebug>
#include <QEvent>
#include <QSettings>
#include <QStyleFactory>
#include <QWidget>

#pragma comment(lib, "Dwmapi.lib")

QtCTProxyStyle::QtCTProxyStyle() {
  QtCT::registerStyleInstance(this);
  QtCTProxyStyle::reloadSettings();
}

QHash<QString, DWM_SYSTEMBACKDROP_TYPE> backdropTypeMap = {
    {"default", DWM_SYSTEMBACKDROP_TYPE::DWMSBT_AUTO},
    {"mica", DWM_SYSTEMBACKDROP_TYPE::DWMSBT_MAINWINDOW},
    {"mica_alt", DWM_SYSTEMBACKDROP_TYPE::DWMSBT_TABBEDWINDOW},
    {"acrylic", DWM_SYSTEMBACKDROP_TYPE::DWMSBT_TRANSIENTWINDOW}};

void QtCTProxyStyle::reloadSettings() {
  QSettings settings(QtCT::configFile(), QSettings::IniFormat);
  QtCT::qt5QSettingsCompatible(settings);
  m_dialogButtonsHaveIcons =
      settings
          .value("Interface/dialog_buttons_have_icons", Qt::PartiallyChecked)
          .toInt();
  m_activateItemOnSingleClick =
      settings
          .value("Interface/activate_item_on_single_click",
                 Qt::PartiallyChecked)
          .toInt();
  m_underlineShortcut =
      settings.value("Interface/underline_shortcut", Qt::PartiallyChecked)
          .toInt();

  QString style =
      settings.value("Appearance/style", "fusion").toString().toLower();
  if (style == "qtct-style" ||
      !QStyleFactory::keys().contains(style, Qt::CaseInsensitive))
    style = "fusion";

  if (style != m_style) {
    setBaseStyle(QStyleFactory::create(style));
    m_style = style;
  }

  settings.beginGroup("Appearance");
  backdrop.type =
      backdropTypeMap[settings.value("backdrop_type", "default").toString()];
  backdrop.darkMode = settings.value("backdrop_dark_mode", false).toBool();
  backdrop.transparency = settings.value("backdrop_transparency", 100).toInt();
}

QtCTProxyStyle::~QtCTProxyStyle() { QtCT::unregisterStyleInstance(this); }

int QtCTProxyStyle::styleHint(QStyle::StyleHint hint,
                              const QStyleOption *option, const QWidget *widget,
                              QStyleHintReturn *returnData) const {
  switch (hint) {
    case QStyle::SH_DialogButtonBox_ButtonsHaveIcons:
      if (m_dialogButtonsHaveIcons == Qt::PartiallyChecked) break;
      return m_dialogButtonsHaveIcons == Qt::Checked;
    case QStyle::SH_ItemView_ActivateItemOnSingleClick:
      if (m_activateItemOnSingleClick == Qt::PartiallyChecked) break;
      return m_activateItemOnSingleClick == Qt::Checked;
    case QStyle::SH_UnderlineShortcut:
      if (m_underlineShortcut == Qt::PartiallyChecked) break;
      return m_underlineShortcut == Qt::Checked;
    default:
      break;
  }
  return QProxyStyle::styleHint(hint, option, widget, returnData);
}

void QtCTProxyStyle::polish(QPalette &pal) {
  QProxyStyle::polish(pal);
  QColor windowColor = QColor(pal.color(QPalette::Window));
  windowColor.setAlphaF(backdrop.transparency / 100.);
  pal.setColor(QPalette::Window, windowColor);
}

MARGINS margins = {-1};
int defaultDarkMode = 0;
auto defaultBackdropType = DWM_SYSTEMBACKDROP_TYPE::DWMSBT_AUTO;

bool isWindow(QWidget *widget) {
  return (widget->windowType() == Qt::Window ||
          widget->windowType() == Qt::Dialog) &&
         !(widget->windowFlags() & Qt::FramelessWindowHint);
}

void QtCTProxyStyle::polish(QApplication *app) {
  app->installEventFilter(this);
  return QProxyStyle::polish(app);
}

void QtCTProxyStyle::unpolish(QApplication *app) {
  app->removeEventFilter(this);

  auto *window = app->activeWindow();
  if (window) {
    HWND hwnd = (HWND)window->winId();
    DwmSetWindowAttribute(hwnd,
                          DWMWINDOWATTRIBUTE::DWMWA_USE_IMMERSIVE_DARK_MODE,
                          &defaultDarkMode, sizeof(defaultDarkMode));
    DwmSetWindowAttribute(hwnd, DWMWINDOWATTRIBUTE::DWMWA_SYSTEMBACKDROP_TYPE,
                          &defaultBackdropType, sizeof(defaultBackdropType));
  }

  return QProxyStyle::unpolish(app);
}

bool QtCTProxyStyle::eventFilter(QObject *watched, QEvent *event) {
  if (watched->isWidgetType() && event->type() == QEvent::WindowActivate) {
    QWidget *widget = qobject_cast<QWidget *>(watched);
    if (isWindow(widget)) {
      applyBackdrop(widget);
    }
  }
  return false;
}

void QtCTProxyStyle::applyBackdrop(QWidget *widget) {
  HWND hwnd = (HWND)widget->winId();

  if (backdrop.type != defaultBackdropType) {
    DwmExtendFrameIntoClientArea(hwnd, &margins);
  }

  int windowDarkMode;
  DwmGetWindowAttribute(hwnd, DWMWINDOWATTRIBUTE::DWMWA_USE_IMMERSIVE_DARK_MODE,
                        &windowDarkMode, sizeof(windowDarkMode));
  if (windowDarkMode != backdrop.darkMode)
    DwmSetWindowAttribute(hwnd,
                          DWMWINDOWATTRIBUTE::DWMWA_USE_IMMERSIVE_DARK_MODE,
                          &backdrop.darkMode, sizeof(backdrop.darkMode));

  DWM_SYSTEMBACKDROP_TYPE windowType;
  DwmGetWindowAttribute(hwnd, DWMWINDOWATTRIBUTE::DWMWA_SYSTEMBACKDROP_TYPE,
                        &windowType, sizeof(windowType));
  if (windowType != backdrop.type)
    DwmSetWindowAttribute(hwnd, DWMWINDOWATTRIBUTE::DWMWA_SYSTEMBACKDROP_TYPE,
                          &backdrop.type, sizeof(backdrop.type));
}
