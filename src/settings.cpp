#include "settings.h"
#include "mainwindow.h"

// Initialize the singleton instance to nullptr
Settings* Settings::s_instance = nullptr;

Settings* Settings::instance() {
    if (!s_instance) {
        s_instance = new Settings();  // Create the singleton instance
    }
    return s_instance;
}

Settings::Settings(QObject* parent)
    : QSettings("Remisa", "Notepad--", parent), m_eol(Unix) {
}

Settings::EOLType Settings::eol() const {
    return m_eol;
}

void Settings::setEol(EOLType eol) {
    if (m_eol != eol) {
        m_eol = eol;
        emit eolChanged(m_eol);
    }
}
