#pragma once

#include <QString>
#include <QPlainTextEdit>
#include <QFile>
#include <QDebug>

class Interpret_As_UTF_16 {
public:
    // Singleton access
    static Interpret_As_UTF_16& instance();

    // Interpret file content as UTF-16 and update the editor
    void execute(QPlainTextEdit* editor);

private:
    // Private constructor and destructor
    Interpret_As_UTF_16();
    ~Interpret_As_UTF_16();

    // Disable copy and assignment
    Interpret_As_UTF_16(const Interpret_As_UTF_16&) = delete;
    Interpret_As_UTF_16& operator=(const Interpret_As_UTF_16&) = delete;

    // Helper function to decode file content
    QString decodeUtf16(const QByteArray& fileContent);
};
