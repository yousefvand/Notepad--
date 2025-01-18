#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_windows_1257 {
public:
    // Singleton instance
    static Interpret_As_windows_1257& instance();

    // Execute decoding for the provided editor
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_windows_1257();
    ~Interpret_As_windows_1257() = default;

    Interpret_As_windows_1257(const Interpret_As_windows_1257&) = delete;
    Interpret_As_windows_1257& operator=(const Interpret_As_windows_1257&) = delete;

    // Decoding function
    QString decodeWindows1257(const QByteArray& rawData);

    // Windows-1257 decoding table
    static const std::unordered_map<unsigned char, QChar> windows1257Table;
};
