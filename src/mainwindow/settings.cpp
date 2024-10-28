#include "settings.h"

// Initialize the static instance of Settings
Settings& Settings::instance() {
    static Settings instance;  // Thread-safe initialization
    return instance;
}

// Private constructor initializing QSettings
Settings::Settings()
    : QSettings("Remisa", "Notepad--") {
}

// Get a setting value by key
QString Settings::getValue(const QString& key, const QString& defaultValue) const {
    return m_settings.value(key, defaultValue).toString();
}

// Set a setting value by key
void Settings::setValue(const QString& key, const QString& value) {
    m_settings.setValue(key, value);
}
