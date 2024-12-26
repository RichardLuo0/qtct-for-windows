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

#ifndef QTCT_H
#define QTCT_H

#define QTCT_TOSTRING(s) #s
#define QTCT_STRINGIFY(maj, min) QTCT_TOSTRING(maj.min)

#define QTCT_VERSION_INT (QTCT_VERSION_MAJOR << 8 | QTCT_VERSION_MINOR)
#define QTCT_VERSION_STR QTCT_STRINGIFY(QTCT_VERSION_MAJOR, QTCT_VERSION_MINOR)

#include <QPalette>
#include <QSet>
#include <QSettings>
#include <QString>
#include <QStringList>

namespace QtCT {
class StyleInstance {
 public:
  virtual void reloadSettings() = 0;
};

void initConfig();
QString configPath();
QString configFile();
QStringList iconPaths();
QString userStyleSheetPath();
QString userColorSchemePath();
QString resolvePath(const QString &path);
QPalette loadColorScheme(const QString &filePath,
                         const QPalette &fallback = QPalette());

void registerStyleInstance(StyleInstance *instance);
void unregisterStyleInstance(StyleInstance *instance);
void reloadStyleInstanceSettings();

void qt5QSettingsCompatible(QSettings &settings);
QFont qt5FontCompatible(const QString &fontAttrStr);
};  // namespace QtCT

#endif  // QTCT_H
