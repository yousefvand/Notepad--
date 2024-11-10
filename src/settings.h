#pragma once

#include <QSettings>
#include <QString>
#include <QVariant>

class Settings : public QSettings {
    Q_OBJECT

public:
    static Settings* instance();  // Singleton accessor

    Settings(const Settings&) = delete;              // Delete copy constructor
    Settings& operator=(const Settings&) = delete;   // Delete assignment operator

    // Save a setting with a given title, name, and value
    void saveSetting(const QString& title, const QString& name, const QVariant& value);

    // Load a setting with a given title and name, providing a default value if it doesn’t exist
    QVariant loadSetting(const QString& title, const QString& name, const QVariant& defaultValue = QVariant()) const;

private:
    explicit Settings(QObject* parent = nullptr);  // Private constructor
    static Settings* s_instance;  // Singleton instance
};
