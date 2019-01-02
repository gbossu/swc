#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QPoint>
#include <QSize>

class Settings
{
public:
    /**
     * Constructor the Settings.
     *
     * It will load the settings in @ref fileName, and use the default settings for missing values.
     *
     * @param fileName The name of the configuration file
     */
    Settings(QString const& fileName = "swc");

    void writeSettings() const;

    int getInt(QString const& key) const;
    QString getString(QString const& key) const;
    QPoint getPoint(QString const& key) const;
    QSize getSize(QString const& key) const;

private:
    QString fileName;
    QMap<QString, QVariant> settingsMap;
};

#endif // SETTINGS_H
