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

#include "mainwindow.h"

#include <QApplication>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QProcessEnvironment>
#include <QSettings>
#include <QStyle>
#include <QStyleFactory>

#include "appearancepage.h"
#include "fontspage.h"
#include "iconthemepage.h"
#include "interfacepage.h"
#include "qsspage.h"
#include "qtct.h"
#include "troubleshootingpage.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), m_ui(new Ui::MainWindow) {
  m_ui->setupUi(this);
  m_ui->tabWidget->addTab(new AppearancePage(this), tr("Appearance"));
  m_ui->tabWidget->addTab(new FontsPage(this), tr("Fonts"));
  m_ui->tabWidget->addTab(new IconThemePage(this), tr("Icon Theme"));
  m_ui->tabWidget->addTab(new InterfacePage(this), tr("Interface"));
#ifdef USE_WIDGETS
  m_ui->tabWidget->addTab(new QSSPage(this), tr("Style Sheets"));
#endif
  m_ui->tabWidget->addTab(new TroubleshootingPage(this), tr("Troubleshooting"));

  QSettings settings(QtCT::configFile(), QSettings::IniFormat);
  restoreGeometry(settings.value("SettingsWindow/geometry").toByteArray());

  setWindowIcon(QIcon::fromTheme("preferences-desktop-theme"));

  m_ui->versionLabel->setText(tr("Version: %1").arg(QTCT_VERSION_STR));
  m_ui->warningIconLabel->setPixmap(
      qApp->style()->standardIcon(QStyle::SP_MessageBoxWarning).pixmap(16, 16));

  checkConfiguration();
  m_ui->warningFrame->setVisible(!m_errors.isEmpty());

  connect(m_ui->buttonBox, &QDialogButtonBox::clicked, this,
          &MainWindow::onButtonBoxClicked);
  connect(m_ui->infoButton, &QPushButton::clicked, this,
          &MainWindow::onInfoButtonClicked);
}

MainWindow::~MainWindow() { delete m_ui; }

void MainWindow::showEvent(QShowEvent *) { m_ui->tabWidget->setFocus(); }

void MainWindow::closeEvent(QCloseEvent *) {
  QSettings settings(QtCT::configFile(), QSettings::IniFormat);
  settings.setValue("SettingsWindow/geometry", saveGeometry());
}

void MainWindow::onButtonBoxClicked(QAbstractButton *button) {
  int id = m_ui->buttonBox->standardButton(button);
  if (id == QDialogButtonBox::Ok || id == QDialogButtonBox::Apply) {
    QSettings settings(QtCT::configFile(), QSettings::IniFormat);
    for (int i = 0; i < m_ui->tabWidget->count(); ++i) {
      TabPage *p = qobject_cast<TabPage *>(m_ui->tabWidget->widget(i));
      if (p) p->writeSettings(&settings);
    }
  }

  if (id == QDialogButtonBox::Ok || id == QDialogButtonBox::Cancel) {
    close();
    qApp->quit();
  }
}

void MainWindow::onInfoButtonClicked() {
  QMessageBox::warning(this, tr("Warning"), m_errors.join("<br><br>"));
}

void MainWindow::checkConfiguration() {
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

  if (!env.contains("QT_QPA_PLATFORMTHEME")) {
    m_errors << tr(
        "The <b>QT_QPA_PLATFORMTHEME</b> environment variable is not set "
        "(required values: <b>qtct</b>, <b>qt5ct</b> or <b>qt6ct</b>).");
  } else if (env.value("QT_QPA_PLATFORMTHEME") != QStringLiteral("qtct") &&
             env.value("QT_QPA_PLATFORMTHEME") != QStringLiteral("qt5ct") &&
             env.value("QT_QPA_PLATFORMTHEME") != QStringLiteral("qt6ct")) {
    m_errors << tr("The <b>QT_QPA_PLATFORMTHEME</b> environment variable is "
                   "not set correctly "
                   "(current value: <b>%1</b>, required values: <b>qtct</b> "
                   "or <b>qt5ct</b>).")
                    .arg(env.value("QT_QPA_PLATFORMTHEME"));
  }

  if (!QStyleFactory::keys().contains("qtct-style")) {
    m_errors << tr("Unable to find <b>qtct-style.dll</b>");
  }

  QVersionNumber v = QLibraryInfo::version();
  if (v.majorVersion() != QT_VERSION_MAJOR ||
      v.minorVersion() != QT_VERSION_MINOR) {
    m_errors << tr("The <b>%1</b> plugin is compiled against incompatible Qt "
                   "version (%2).")
                    .arg("libqtct.so", QT_VERSION_STR);
  }
}
