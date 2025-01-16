#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_windows_1250 {
public:
    // Singleton instance access
    static Interpret_As_windows_1250& instance();

    // Execute decoding for the provided editor
    void execute(QPlainTextEdit* editor);

private:
    // Private constructor for Singleton pattern
    Interpret_As_windows_1250();
    ~Interpret_As_windows_1250() = default;

    // Disable copy construction and assignment
    Interpret_As_windows_1250(const Interpret_As_windows_1250&) = delete;
    Interpret_As_windows_1250& operator=(const Interpret_As_windows_1250&) = delete;

    // Decoding method for Windows-1250 data
    QString decodeWindows1250(const QByteArray& rawData);

    // Windows-1250 encoding table
    static const std::unordered_map<unsigned char, QChar> windows1250Table;
};
