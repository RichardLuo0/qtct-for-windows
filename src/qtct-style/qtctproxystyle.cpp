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

#include <QSettings>
#include <QStyleFactory>
#include <QWidget>

#pragma comment(lib, "Dwmapi.lib")

QtCTProxyStyle::QtCTProxyStyle() {
  QtCT::registerStyleInstance(this);
  QtCTProxyStyle::reloadSettings();
}

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

// QSet<HWND> enabledWindowList;

/* MARGINS margins = {-1};
int darkMode = true;
auto mica_value = DWM_SYSTEMBACKDROP_TYPE::DWMSBT_TRANSIENTWINDOW; */

// void QtCTProxyStyle::polish(QWidget *widget) {
//   if (!enabledWindowList.contains((HWND)widget->winId())) {
//     HWND hwnd = (HWND)widget->winId();
//     enabledWindowList.insert(hwnd);

//     widget->setStyleSheet("background:transparent");

//     DwmExtendFrameIntoClientArea(hwnd, &margins);
//     DwmSetWindowAttribute(hwnd,
//                           DWMWINDOWATTRIBUTE::DWMWA_USE_IMMERSIVE_DARK_MODE,
//                           &darkMode, sizeof(int));
//     DwmSetWindowAttribute(hwnd,
//     DWMWINDOWATTRIBUTE::DWMWA_SYSTEMBACKDROP_TYPE,
//                           &mica_value, sizeof(mica_value));
//   }

//   return QProxyStyle::polish(widget);
// }
