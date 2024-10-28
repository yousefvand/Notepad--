#pragma once

#include <QSettings>
#include <QString>

class Settings : public QSettings {
public:
    // Provide a global point of access to the instance
    static Settings& instance();

    // Delete copy constructor and assignment operator to prevent multiple instances
    Settings(const Settings&) = delete;
    Settings& operator=(const Settings&) = delete;

    // Functions to manage settings
    QString getValue(const QString& key, const QString& defaultValue = QString()) const;
    void setValue(const QString& key, const QString& value);

private:
    // Private constructor to prevent instantiation from outside
    Settings();
    QSettings m_settings;
};
