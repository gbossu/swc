#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QPoint>
#include <QSize>
#include <QFile>

class Settings
{
public:
    /**
     * Constructor for Settings.
     *
     * It will load the settings in @ref file, and use the default settings
     * for missing values.
     *
     * @param file The file used to read configuration file
     */
    Settings(const QFile &file);

    /**
     * Constructor for Settings.
     *
     * It will try to load the specific settings for the given swc-key, or it will
     * load the default settings file if there are no specific settings.
     *
     * The default settings will be used for missing values.
     *
     * @param swcKey The unique identifier of the SWC container
     */
    Settings(QString const& swcKey);

    /**
     * Write the settings contained in the map to a QSettings file.
     *
     * If the files contained some settings, they will be erased.
     *
     * @param settings The QSettings object used to write the settings in a file
     */
    void writeSettings(QSettings &settings) const;

    bool getBool(QString const& key) const;
    int getInt(QString const& key) const;
    uint getUInt(QString const& key) const;
    QString getString(QString const& key) const;
    QPoint getPoint(QString const& key) const;
    QSize getSize(QString const& key) const;
    Qt::WindowFlags getWindowFlags() const;

    void setValue(QString const& key, QVariant const& value);

private:
    void generateDefaultsAndMerge(QSettings const& settings);

    QMap<QString, QVariant> settingsMap;
};

#endif // SETTINGS_H
