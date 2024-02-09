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

#include "qtct.h"

#include <QDir>
#include <QFile>
#include <QFont>
#include <QLatin1String>
#include <QLocale>
#include <QRegularExpression>
#include <QSettings>
#include <QStandardPaths>
#include <QString>
#include <QtDebug>

QSet<QtCT::StyleInstance *> styleInstances;

void QtCT::initConfig() {
  if (QFile::exists(configFile())) return;

  QString globalConfig = QStandardPaths::locate(
      QStandardPaths::GenericConfigLocation, "qtct/qtct.conf");
  if (globalConfig.isEmpty()) return;

  QDir("/").mkpath(configPath());
  QFile::copy(globalConfig, configFile());
}

QString QtCT::configPath() {
  return QStandardPaths::writableLocation(
             QStandardPaths::GenericConfigLocation) +
         QLatin1String("/qtct");
}

QString QtCT::configFile() {
  return configPath() + QLatin1String("/qtct.conf");
}

QStringList QtCT::iconPaths() {
  QStringList paths = {QDir::homePath() + QLatin1String("/.icons")};

  const auto &genericDataPathList =
      QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
  for (const QString &p : genericDataPathList) {
    paths << (p + QLatin1String("/icons"));
  }
  paths.removeDuplicates();

  // remove invalid
  QStringList::iterator it = paths.begin();
  while (it != paths.end()) {
    if (QDir(*it).exists())
      ++it;
    else
      it = paths.erase(it);
  }

  return paths;
}

QString QtCT::userStyleSheetPath() {
  return configPath() + QLatin1String("/qss");
}

QString QtCT::userColorSchemePath() {
  return configPath() + QLatin1String("/colors");
}

QString QtCT::resolvePath(const QString &path) {
  QString tmp = path;
  tmp.replace("~",
              QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
  if (!tmp.contains("$")) return tmp;

  // find environment variables
  static const QRegularExpression regexp("\\$([A-Z_]+)\\/");
  QRegularExpressionMatchIterator it = regexp.globalMatch(tmp);

  while (it.hasNext()) {
    QRegularExpressionMatch match = it.next();
    QString captured = match.captured(1);
    tmp.replace(QLatin1String("$") + captured,
                qgetenv(captured.toLatin1().constData()));
  }

  return tmp;
}

QPalette QtCT::loadColorScheme(const QString &filePath) {
  QPalette customPalette;
  QSettings settings(filePath, QSettings::IniFormat);
  settings.beginGroup("ColorScheme");
  QStringList activeColors = settings.value("active_colors").toStringList();
  QStringList inactiveColors = settings.value("inactive_colors").toStringList();
  QStringList disabledColors = settings.value("disabled_colors").toStringList();
  settings.endGroup();

#if (QT_VERSION >= QT_VERSION_CHECK(6, 6, 0))
  if (activeColors.count() == QPalette::Accent)
    activeColors << activeColors.at(QPalette::Highlight);
  if (inactiveColors.count() == QPalette::Accent)
    inactiveColors << inactiveColors.at(QPalette::Highlight);
  if (disabledColors.count() == QPalette::Accent)
    disabledColors << disabledColors.at(QPalette::Highlight);
#endif

  if (activeColors.count() >= QPalette::NColorRoles &&
      inactiveColors.count() >= QPalette::NColorRoles &&
      disabledColors.count() >= QPalette::NColorRoles) {
    for (int i = 0; i < QPalette::NColorRoles; i++) {
      QPalette::ColorRole role = QPalette::ColorRole(i);
      customPalette.setColor(QPalette::Active, role,
                             QColor(activeColors.at(i)));
      customPalette.setColor(QPalette::Inactive, role,
                             QColor(inactiveColors.at(i)));
      customPalette.setColor(QPalette::Disabled, role,
                             QColor(disabledColors.at(i)));
    }
  }
  return customPalette;
}

void QtCT::registerStyleInstance(QtCT::StyleInstance *instance) {
  styleInstances.insert(instance);
}

void QtCT::unregisterStyleInstance(QtCT::StyleInstance *instance) {
  styleInstances.remove(instance);
}

void QtCT::reloadStyleInstanceSettings() {
  for (auto instance : qAsConst(styleInstances)) instance->reloadSettings();
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QTextCodec>
#endif

void QtCT::qt5QSettingsCompatible(QSettings &settings) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  settings.setIniCodec(QTextCodec::codecForName("UTF-8"));
#else
  Q_UNUSED(settings);
#endif
}

QHash<QString, QFont::Weight> openTypeWeightMap{
    {"100", QFont::Thin},   {"200", QFont::ExtraLight},
    {"300", QFont::Light},  {"400", QFont::Normal},
    {"500", QFont::Medium}, {"600", QFont::DemiBold},
    {"700", QFont::Bold},   {"800", QFont::ExtraBold},
    {"900", QFont::Black}};

QFont QtCT::qt5FontCompatible(const QString &fontAttrStr) {
  QFont font;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  QStringList fontAttrList = fontAttrStr.split(",");
  fontAttrList[4] = QString::number(openTypeWeightMap[fontAttrList[4]]);
  QString str;
  for (int i = 0; i < 10; ++i) {
    str += fontAttrList[i] + ",";
  }
  str += fontAttrList.back();
  font.fromString(str);
#else
  font.fromString(fontAttrStr);
#endif
  return font;
}
