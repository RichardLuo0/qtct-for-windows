#ifndef SHAREDPATH_H
#define SHAREDPATH_H

#include <QCoreApplication>
#include <QStandardPaths>
#include <QStringList>

inline QStringList sharedColorSchemePaths() {
  QStringList paths;
  const auto &genericDataPathList =
      QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
  for (const QString &p : genericDataPathList) {
    paths << (p + QLatin1String("/qtct/colors"));
  }
  paths << (QCoreApplication::applicationDirPath() +
            QLatin1String("/share/colors"));
  paths.removeDuplicates();
  return paths;
}

inline QStringList sharedStyleSheetPaths() {
  QStringList paths;
  const auto &genericDataPathList =
      QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
  for (const QString &p : genericDataPathList) {
    paths << (p + QLatin1String("/qtct/qss"));
  }
  paths << (QCoreApplication::applicationDirPath() +
            QLatin1String("/share/colors"));
  paths.removeDuplicates();
  return paths;
}
#endif  // SHAREDPATH_H
