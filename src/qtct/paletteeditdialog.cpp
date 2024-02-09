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

#include "paletteeditdialog.h"

#include <QColorDialog>
#include <QPalette>
#include <QSettings>

#include "QtCT.h"
#include "ui_paletteeditdialog.h"

PaletteEditDialog::PaletteEditDialog(const QPalette &palette,
                                     QStyle *currentStyle, QWidget *parent)
    : QDialog(parent), m_ui(new Ui::PaletteEditDialog) {
  m_currentStyle = currentStyle;
  m_ui->setupUi(this);
  m_ui->tableWidget->setColumnCount(3);
  m_ui->tableWidget->setRowCount(QPalette::NColorRoles);
  m_ui->tableWidget->verticalHeader()->setDefaultSectionSize(
      fontMetrics().lineSpacing() + 10);
  m_ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  m_ui->tableWidget->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);

  QStringList labels = {tr("Active"), tr("Inactive"), tr("Disabled")};
  m_ui->tableWidget->setHorizontalHeaderLabels(labels);
  setPalette(palette);

  QSettings settings(QtCT::configFile(), QSettings::IniFormat);
  restoreGeometry(settings.value("PaletteEditor/geometry").toByteArray());

  connect(m_ui->tableWidget, &QTableWidget::itemClicked, this,
          &PaletteEditDialog::onTableWidgetItemClicked);
  connect(m_ui->resetPaletteButton, &QPushButton::clicked, this,
          &PaletteEditDialog::onResetPaletteButtonClicked);
  connect(m_ui->buildInactiveButton, &QPushButton::clicked, this,
          &PaletteEditDialog::onBuildInactiveButtonClicked);
  connect(m_ui->buildDisabledButton, &QPushButton::clicked, this,
          &PaletteEditDialog::onBuildDisabledButtonClicked);
}

PaletteEditDialog::~PaletteEditDialog() { delete m_ui; }

QPalette PaletteEditDialog::selectedPalette() const {
  QPalette palette;
  for (int i = 0; i < QPalette::NColorRoles; i++) {
    palette.setBrush(QPalette::Active, QPalette::ColorRole(i),
                     m_ui->tableWidget->item(i, 0)->background());
    palette.setBrush(QPalette::Inactive, QPalette::ColorRole(i),
                     m_ui->tableWidget->item(i, 1)->background());
    palette.setBrush(QPalette::Disabled, QPalette::ColorRole(i),
                     m_ui->tableWidget->item(i, 2)->background());
  }
  return palette;
}

void PaletteEditDialog::setPalette(const QPalette &palette) {
  for (int i = 0; i < QPalette::NColorRoles; i++) {
    if (!m_ui->tableWidget->item(i, 0))
      m_ui->tableWidget->setItem(i, 0, new QTableWidgetItem());
    if (!m_ui->tableWidget->item(i, 1))
      m_ui->tableWidget->setItem(i, 1, new QTableWidgetItem());
    if (!m_ui->tableWidget->item(i, 2))
      m_ui->tableWidget->setItem(i, 2, new QTableWidgetItem());

    m_ui->tableWidget->item(i, 0)->setBackground(
        palette.color(QPalette::Active, QPalette::ColorRole(i)));
    m_ui->tableWidget->item(i, 1)->setBackground(
        palette.color(QPalette::Inactive, QPalette::ColorRole(i)));
    m_ui->tableWidget->item(i, 2)->setBackground(
        palette.color(QPalette::Disabled, QPalette::ColorRole(i)));
  }

  QStringList labels = {tr("Window text"),
                        tr("Button background"),
                        tr("Bright"),
                        tr("Less bright"),
                        tr("Dark"),
                        tr("Less dark"),
                        tr("Normal text"),
                        tr("Bright text"),
                        tr("Button text"),
                        tr("Normal background"),
                        tr("Window"),
                        tr("Shadow"),
                        tr("Highlight"),
                        tr("Highlighted text"),
                        tr("Link"),
                        tr("Visited link"),
                        tr("Alternate background"),
                        tr("Default"),
                        tr("Tooltip background"),
                        tr("Tooltip text"),
                        tr("Placeholder text")};

#if (QT_VERSION >= QT_VERSION_CHECK(6, 6, 0))
  labels << tr("Accent");
#endif
  m_ui->tableWidget->setVerticalHeaderLabels(labels);
}

void PaletteEditDialog::hideEvent(QHideEvent *) {
  QSettings settings(QtCT::configFile(), QSettings::IniFormat);
  settings.setValue("PaletteEditor/geometry", saveGeometry());
}

void PaletteEditDialog::onTableWidgetItemClicked(QTableWidgetItem *item) {
  QColor color = QColorDialog::getColor(item->background().color(), this,
                                        tr("Select Color"),
                                        QColorDialog::ShowAlphaChannel);
  if (color.isValid()) {
    item->setBackground(color);
    emit paletteChanged(selectedPalette());
  }
}

void PaletteEditDialog::onResetPaletteButtonClicked() {
  setPalette(m_currentStyle->standardPalette());
  emit paletteChanged(selectedPalette());
}

void PaletteEditDialog::onBuildInactiveButtonClicked() {
  QPalette palette = selectedPalette();
  for (int i = 0; i < QPalette::NColorRoles; i++) {
    palette.setColor(QPalette::Inactive, QPalette::ColorRole(i),
                     palette.color(QPalette::Active, QPalette::ColorRole(i)));
  }
  setPalette(palette);
  emit paletteChanged(selectedPalette());
}

void PaletteEditDialog::onBuildDisabledButtonClicked() {
  QPalette palette = selectedPalette();
  for (int i = 0; i < QPalette::NColorRoles; i++) {
    palette.setColor(QPalette::Disabled, QPalette::ColorRole(i),
                     palette.color(QPalette::Active, QPalette::ColorRole(i)));
  }
  palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
  palette.setColor(QPalette::Disabled, QPalette::WindowText, Qt::darkGray);
  palette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
  palette.setColor(QPalette::Disabled, QPalette::HighlightedText, Qt::darkGray);
  setPalette(palette);
  emit paletteChanged(selectedPalette());
}
