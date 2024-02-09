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

#include "fontspage.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFontDialog>
#include <QMessageBox>
#include <QSettings>

#include "fontconfigdialog.h"
#include "qtct.h"
#include "ui_fontspage.h"

FontsPage::FontsPage(QWidget *parent)
    : TabPage(parent), m_ui(new Ui::FontsPage) {
  m_ui->setupUi(this);

  fontLabels[General] = m_ui->generalFontLabel;
  fontLabels[Fixed] = m_ui->fixedFontLabel;

  connect(m_ui->fontOverrideCheckbox, &QCheckBox::stateChanged, this,
          [=](int state) { m_ui->container->setEnabled(state); });

  connect(m_ui->changeGeneralFontButton, &QToolButton::clicked, this,
          [=] { onFontChangeRequested(General); });
  connect(m_ui->changeFixedWidthFontButton, &QToolButton::clicked, this,
          [=] { onFontChangeRequested(Fixed); });

  readSettings();

  // icons
  m_ui->createFontsConfButton->setIcon(QIcon::fromTheme("document-new"));
  m_ui->removeFontsConfButton->setIcon(QIcon::fromTheme("edit-delete"));

  connect(m_ui->createFontsConfButton, &QPushButton::clicked, this,
          &FontsPage::onCreateFontsConfButtonClicked);
  connect(m_ui->removeFontsConfButton, &QPushButton::clicked, this,
          &FontsPage::onRemoveFontsConfButtonClicked);
}

FontsPage::~FontsPage() { delete m_ui; }

void FontsPage::writeSettings(QSettings *settings) {
  settings->beginGroup("Fonts");
  settings->setValue("font_override", m_ui->fontOverrideCheckbox->isChecked());
  for (int i = 0; i < NFonts; ++i) {
    settings->setValue(settingsKeys[i], fonts[i].toString());
  }
  settings->endGroup();
}

void FontsPage::onFontChangeRequested(FontType type) {
  bool ok = false;
  QFont font = QFontDialog::getFont(&ok, fonts[type], this);
  if (ok) {
    fontLabels[type]->setFont(font);
    fontLabels[type]->setText(font.family() + " " +
                              QString::number(font.pointSize()));
    fonts[type] = font;
  }
}

void FontsPage::readSettings() {
  QSettings settings(QtCT::configFile(), QSettings::IniFormat);
  settings.beginGroup("Fonts");
  m_ui->fontOverrideCheckbox->setChecked(
      settings.value("font_override", false).toBool());
  for (int i = 0; i < NFonts; ++i) {
    QLabel *label = fontLabels[i];
    QFont font = QApplication::font();
    font.fromString(
        settings.value(settingsKeys[i], QApplication::font().toString())
            .toString());
    label->setText(font.family() + " " + QString::number(font.pointSize()));
    label->setFont(font);
    fonts[i] = font;
  }
  settings.endGroup();
}

void FontsPage::onCreateFontsConfButtonClicked() {
  FontConfigDialog d(this);
  d.exec();
}

void FontsPage::onRemoveFontsConfButtonClicked() {
  QString path = QDir::homePath() + "/.config/fontconfig/fonts.conf";

  if (QFile::exists(path)) {
    if (QMessageBox::question(
            this, tr("Remove Font Configuration"),
            tr("Are you sure you want to delete <i>%1</i>?").arg(path),
            QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
      return;
    }

    QFile::remove(path + ".back");
    QFile::copy(path, path + ".back");
    QFile::remove(path);
  }
}
