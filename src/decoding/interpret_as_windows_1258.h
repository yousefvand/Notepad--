#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_windows_1258 {
public:
    // Singleton instance
    static Interpret_As_windows_1258& instance();

    // Execute decoding for the provided editor
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_windows_1258();
    ~Interpret_As_windows_1258() = default;

    Interpret_As_windows_1258(const Interpret_As_windows_1258&) = delete;
    Interpret_As_windows_1258& operator=(const Interpret_As_windows_1258&) = delete;

    // Decoding function
    QString decodeWindows1258(const QByteArray& rawData);

    // Windows-1258 decoding table
    static const std::unordered_map<unsigned char, QChar> windows1258Table;
};
