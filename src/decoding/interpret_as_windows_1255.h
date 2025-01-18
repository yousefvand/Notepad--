#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_windows_1255 {
public:
    // Singleton instance
    static Interpret_As_windows_1255& instance();

    // Execute decoding for the provided editor
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_windows_1255();
    ~Interpret_As_windows_1255() = default;

    Interpret_As_windows_1255(const Interpret_As_windows_1255&) = delete;
    Interpret_As_windows_1255& operator=(const Interpret_As_windows_1255&) = delete;

    // Decoding function
    QString decodeWindows1255(const QByteArray& rawData);

    // Windows-1255 decoding table
    static const std::unordered_map<unsigned char, QChar> windows1255Table;
};
