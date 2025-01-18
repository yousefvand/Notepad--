#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_windows_1253 {
public:
    // Singleton instance
    static Interpret_As_windows_1253& instance();

    // Execute decoding for the provided editor
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_windows_1253();
    ~Interpret_As_windows_1253() = default;

    Interpret_As_windows_1253(const Interpret_As_windows_1253&) = delete;
    Interpret_As_windows_1253& operator=(const Interpret_As_windows_1253&) = delete;

    // Decoding function
    QString decodeWindows1253(const QByteArray& rawData);

    // Windows-1253 decoding table
    static const std::unordered_map<unsigned char, QChar> windows1253Table;
};

