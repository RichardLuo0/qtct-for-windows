#ifndef QWINDOWSTHEME_H
#define QWINDOWSTHEME_H

#include <private/qguiapplication_p.h>
#include <qpa/qplatformintegration.h>
#include <qpa/qplatformtheme.h>

class QWindowsTheme : public QPlatformTheme {
 private:
  QScopedPointer<QPlatformTheme> winTheme;

 public:
  QWindowsTheme() {
    winTheme.reset(
        QGuiApplicationPrivate::platform_integration->createPlatformTheme(
            "windows"));
  }

  virtual ~QWindowsTheme() override {}

  QPlatformMenuItem *createPlatformMenuItem() const override {
    return winTheme->createPlatformMenuItem();
  };
  virtual QPlatformMenu *createPlatformMenu() const override {
    return winTheme->createPlatformMenu();
  };
  virtual QPlatformMenuBar *createPlatformMenuBar() const override {
    return winTheme->createPlatformMenuBar();
  };
  virtual void showPlatformMenuBar() override {
    return winTheme->showPlatformMenuBar();
  };

  virtual bool usePlatformNativeDialog(
      DialogType type) const override {
    return winTheme->usePlatformNativeDialog(type);
  };
  virtual QPlatformDialogHelper *createPlatformDialogHelper(
      DialogType type) const override {
    return winTheme->createPlatformDialogHelper(type);
  };

  virtual QPlatformSystemTrayIcon *createPlatformSystemTrayIcon()
      const override {
    return winTheme->createPlatformSystemTrayIcon();
  };

  virtual Qt::ColorScheme colorScheme() const override {
    return winTheme->colorScheme();
  }

  virtual const QPalette *palette(
      Palette type = SystemPalette) const override {
    return winTheme->palette(type);
  }

  virtual const QFont *font(
      Font type = SystemFont) const override {
    return winTheme->font(type);
  }

  virtual QVariant themeHint(
      ThemeHint hint) const override {
    return winTheme->themeHint(hint);
  }

  virtual QPixmap standardPixmap(
      StandardPixmap sp, const QSizeF &size) const override {
    return winTheme->standardPixmap(sp, size);
  }
  virtual QIcon fileIcon(
      const QFileInfo &fileInfo,
      QPlatformTheme::IconOptions iconOptions = {}) const override {
    return winTheme->fileIcon(fileInfo, iconOptions);
  }
  virtual QIconEngine *createIconEngine(
      const QString &iconName) const override {
    return winTheme->createIconEngine(iconName);
  }

  virtual QList<QKeySequence> keyBindings(
      QKeySequence::StandardKey key) const override {
    return winTheme->keyBindings(key);
  }

  virtual QString standardButtonText(
      int button) const override {
    return winTheme->standardButtonText(button);
  }
  virtual QKeySequence standardButtonShortcut(
      int button) const override {
    return winTheme->standardButtonShortcut(button);
  }
};

#endif  // QWINDOWSTHEME_H
