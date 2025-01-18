#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_windows_1251 {
public:
    // Singleton instance access
    static Interpret_As_windows_1251& instance();

    // Execute decoding for the provided editor
    void execute(QPlainTextEdit* editor);

private:
    // Private constructor for Singleton pattern
    Interpret_As_windows_1251();
    ~Interpret_As_windows_1251() = default;

    // Disable copy construction and assignment
    Interpret_As_windows_1251(const Interpret_As_windows_1251&) = delete;
    Interpret_As_windows_1251& operator=(const Interpret_As_windows_1251&) = delete;

    // Decoding method for Windows-1251 data
    QString decodeWindows1251(const QByteArray& rawData);

    // Windows-1251 encoding table
    static const std::unordered_map<unsigned char, QChar> windows1251Table;
};
