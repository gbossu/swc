#include "Settings.h"
#include <QStandardPaths>
#include <QFileInfo>

Settings::Settings(const QString &swcKey)
{
    // If there is a specific contig file for this swc id, use it
    //TODO: use more efficient string concatenation or keep it this way
    // for readability
    auto filePath =
            QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)
            .append(QString("/swc/keys/")).append(swcKey).append(".conf");
    auto fileName = QFileInfo(filePath).exists() ?
                    QString("keys/").append(swcKey) :
                    "swc";

    // Fill the settings map with the default values
    // And merge with the read settings
    QSettings settings("swc", fileName);
    generateDefaultsAndMerge(settings);

    // Write the settings if the default file does not exist
    if (!QFileInfo(settings.fileName()).exists())
        writeSettings(settings);
}

void Settings::generateDefaultsAndMerge(const QSettings &settings)
{
    // Default settings for the container
    settingsMap["container/position_type"] = "percent";
    settingsMap["container/position"] = QPoint(50, 0);
    settingsMap["container/offset"] = QPoint(0, 0);
    settingsMap["container/size_type"] = "auto";
    settingsMap["container/size"] = QSize(100, 100); // Some dummy size

    // Default settings for the animation
    settingsMap["animation/direction"] = "down";
    settingsMap["animation/duration"] = 400;

    // Default settings for the window lookup
    settingsMap["lookup/max_tries"] = 25;
    settingsMap["lookup/try_interval"] = 1;

    // Default ownership rules for handling the process/window
    settingsMap["ownership/pause"] = false;

    // Overwrite using the given settings
    QStringList keys = settings.allKeys();
    for (QString const& i : keys)
        settingsMap[i] = settings.value(i);
}

void Settings::writeSettings(QSettings &settings) const
{
    settings.clear();

    for (auto it = settingsMap.begin(); it != settingsMap.end(); ++it) {
        settings.setValue(it.key(), it.value());
    }
}

bool Settings::getBool(const QString &key) const
{
    return settingsMap[key].toBool();
}

int Settings::getInt(const QString &key) const
{
    return settingsMap[key].toInt();
}

uint Settings::getUInt(const QString &key) const
{
    return settingsMap[key].toUInt();
}

QString Settings::getString(const QString &key) const
{
    return settingsMap[key].toString();
}

QPoint Settings::getPoint(const QString &key) const
{
    return settingsMap[key].toPoint();
}

QSize Settings::getSize(const QString &key) const
{
    return settingsMap[key].toSize();
}

void Settings::setValue(const QString &key, const QVariant &value)
{
    settingsMap[key] = value;
}
