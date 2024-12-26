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

#ifndef QTCTPLATFORMTHEME_H
#define QTCTPLATFORMTHEME_H

#include <QFileInfo>
#include <QFont>
#include <QIcon>
#include <QLoggingCategory>
#include <QObject>
#include <QPalette>
#include <QScopedPointer>

#include "qwindowstheme.h"

Q_DECLARE_LOGGING_CATEGORY(
    lqtct)

class QtCTPlatformTheme : public QObject, public QWindowsTheme {
  Q_OBJECT
 public:
  QtCTPlatformTheme();

  ~QtCTPlatformTheme() override;

  bool usePlatformNativeDialog(DialogType type) const override;
  QPlatformDialogHelper *createPlatformDialogHelper(
      DialogType type) const override;

  const QPalette *palette(Palette type = SystemPalette) const override;

  const QFont *font(Font type = SystemFont) const override;  // cpp

  QVariant themeHint(ThemeHint hint) const override;  // cpp

  QIcon fileIcon(
      const QFileInfo &fileInfo,
      QPlatformTheme::IconOptions iconOptions = {}) const override;  // cpp

 private slots:
  void applySettings();
#ifdef QT_WIDGETS_LIB
  void createFSWatcher();
  void updateSettings();
#endif

 private:
  void readSettings();
#ifdef QT_WIDGETS_LIB
  bool hasWidgets();
#endif
  QString loadStyleSheets(const QStringList &paths);
  QString m_style, m_iconTheme, m_userStyleSheet, m_prevStyleSheet;
  std::unique_ptr<QPalette> m_palette;
  bool m_fontOverride = false;
  QFont m_generalFont, m_fixedFont;
  int m_doubleClickInterval;
  int m_cursorFlashTime;
  int m_uiEffects;
  int m_buttonBoxLayout;
  int m_keyboardScheme;
  bool m_update = false;
  bool m_usePalette = true;
  int m_toolButtonStyle = Qt::ToolButtonFollowStyle;
  int m_wheelScrollLines = 3;
  bool m_showShortcutsInContextMenus = false;
  bool m_isIgnored = false;
  QScopedPointer<QPlatformTheme> m_theme;
};

Q_DECLARE_LOGGING_CATEGORY(
    lqtct)

#endif  // QTCTPLATFORMTHEME_H
