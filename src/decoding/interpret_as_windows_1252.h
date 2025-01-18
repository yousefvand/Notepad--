#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_windows_1252 {
public:
    // Singleton instance
    static Interpret_As_windows_1252& instance();

    // Execute decoding for the provided editor
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_windows_1252();
    ~Interpret_As_windows_1252() = default;

    Interpret_As_windows_1252(const Interpret_As_windows_1252&) = delete;
    Interpret_As_windows_1252& operator=(const Interpret_As_windows_1252&) = delete;

    // Decoding function
    QString decodeWindows1252(const QByteArray& rawData);

    // Windows-1252 decoding table
    static const std::unordered_map<unsigned char, QChar> windows1252Table;
};
