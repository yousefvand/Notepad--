#pragma once

#include <QString>
#include <QPlainTextEdit>
#include <QFile>
#include <QDebug>

class Interpret_As_US_ASCII {
public:
    // Singleton access
    static Interpret_As_US_ASCII& instance();

    // Interpret file content as US-ASCII and update the editor
    void execute(QPlainTextEdit* editor);

private:
    // Private constructor and destructor
    Interpret_As_US_ASCII();
    ~Interpret_As_US_ASCII();

    // Disable copy and assignment
    Interpret_As_US_ASCII(const Interpret_As_US_ASCII&) = delete;
    Interpret_As_US_ASCII& operator=(const Interpret_As_US_ASCII&) = delete;

    // Helper function to decode file content
    QString decodeUsAscii(const QByteArray& fileContent);
};
