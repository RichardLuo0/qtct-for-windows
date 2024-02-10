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

#ifndef ICONTHEMEPAGE_H
#define ICONTHEMEPAGE_H

#include <QIcon>
#include <atomic>

#include "tabpage.h"

namespace Ui {
class IconThemePage;
}

class QTreeWidgetItem;
class QProgressBar;
class QThread;

class IconThemePage : public TabPage {
  Q_OBJECT

 public:
  explicit IconThemePage(QWidget *parent = nullptr);
  ~IconThemePage();

  void writeSettings(QSettings *settings) override;

 private slots:
  void onFinished();

 private:
  void resizeEvent(QResizeEvent *event) override;
  void readSettings();
  void loadThemes();
  QTreeWidgetItem *loadTheme(const QString &path);
  QIcon findIcon(const QString &themePath, int size, const QString &name);
  QIcon findIconHelper(const QString &themePath, int size, const QString &name,
                       QStringList *visited);
  Ui::IconThemePage *m_ui;
  QProgressBar *m_progressBar;
  QThread *m_thread;
  QList<QTreeWidgetItem *> m_items;
  std::atomic_bool m_stopped = ATOMIC_VAR_INIT(false);
};

#endif  // ICONTHEMEPAGE_H