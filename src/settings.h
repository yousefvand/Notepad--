#pragma once

#include <QSettings>
#include <QString>

class Settings : public QSettings {
    Q_OBJECT

public:
    static Settings* instance();  // Singleton accessor

    Settings(const Settings&) = delete;              // Delete copy constructor
    Settings& operator=(const Settings&) = delete;   // Delete assignment operator

    enum EOLType { Unix, Windows, OldMac };
    Q_ENUM(EOLType)

    EOLType eol() const;
    void setEol(EOLType eol);

signals:
    void eolChanged(EOLType newEOL);

private:
    explicit Settings(QObject* parent = nullptr);  // Private constructor
    static Settings* s_instance;  // Singleton instance

    EOLType m_eol;
};
