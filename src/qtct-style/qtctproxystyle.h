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

#ifndef QTCTPROXYSTYLE_H
#define QTCTPROXYSTYLE_H

#include <QProxyStyle>

#include "qtct.h"

struct Backdrop {
  int type = -1;
  int darkMode;
  int opacity;
};

class QtCTProxyStyle : public QProxyStyle, public QtCT::StyleInstance {
  Q_OBJECT
 public:
  explicit QtCTProxyStyle();
  void reloadSettings() override;

  virtual ~QtCTProxyStyle();

  int styleHint(StyleHint hint, const QStyleOption *option,
                const QWidget *widget,
                QStyleHintReturn *returnData) const override;

  void polish(QPalette &pal) override;
  void polish(QApplication *app) override;
  void unpolish(QApplication *app) override;

  bool eventFilter(QObject *watched, QEvent *event) override;

 private:
  QString m_style;
  int m_dialogButtonsHaveIcons;
  int m_activateItemOnSingleClick;
  int m_underlineShortcut;

  Backdrop backdrop;

  void applyBackdrop(QWidget *widget);
};

#endif  // QTCTPROXYSTYLE_H
