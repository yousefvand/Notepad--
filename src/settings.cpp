#include "settings.h"

// Initialize the singleton instance to nullptr
Settings* Settings::s_instance = nullptr;

Settings* Settings::instance() {
    if (!s_instance) {
        s_instance = new Settings();  // Create the singleton instance
    }
    return s_instance;
}

Settings::Settings(QObject* parent)
    : QSettings("Remisa", "Notepad--", parent) {}

// Save a setting with a given title and name
void Settings::saveSetting(const QString& title, const QString& name, const QVariant& value) {
    QString key = title + "/" + name;  // Construct the key as "title/name"
    setValue(key, value);
}

// Load a setting with a given title and name, providing a default value
QVariant Settings::loadSetting(const QString& title, const QString& name, const QVariant& defaultValue) const {
    QString key = title + "/" + name;  // Construct the key as "title/name"
    return value(key, defaultValue);
}
