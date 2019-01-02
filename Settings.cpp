#include "Settings.h"
#include <QFile>

Settings::Settings(const QString &fileName) :
    fileName(fileName)
{
    // Default settings for the container
    settingsMap["container/position_type"] = "percent";
    settingsMap["container/position"] = QPoint(50, 0);
    settingsMap["container/offset"] = QPoint(0, 0);

    // Default settings for the animation
    settingsMap["animation/direction"] = "down";
    settingsMap["animation/duration"] = 400;

    // Default settings for the window lookup
    settingsMap["lookup/max_tries"] = 25;
    settingsMap["lookup/try_interval"] = 1;

    QSettings settings(QSettings::NativeFormat, QSettings::UserScope, "swc", fileName);
    QFile settingsFile(settings.fileName());
    if (!settingsFile.exists())
        writeSettings();

    QStringList keys = settings.allKeys();
    for (QString const& i : keys)
        settingsMap[i] = settings.value(i);
}

void Settings::writeSettings() const
{
    QSettings settings(QSettings::NativeFormat, QSettings::UserScope, "swc", fileName);
    settings.clear();

    for (auto it = settingsMap.begin(); it != settingsMap.end(); ++it) {
        settings.setValue(it.key(), it.value());
    }
}

int Settings::getInt(const QString &key) const
{
    return settingsMap[key].toInt();
}

QString Settings::getString(const QString &key) const
{
    return settingsMap[key].toString();
}

QPoint Settings::getPoint(const QString &key) const
{
    return settingsMap[key].toPoint();
}
