#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_windows_1254 {
public:
    // Singleton instance
    static Interpret_As_windows_1254& instance();

    // Execute decoding for the provided editor
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_windows_1254();
    ~Interpret_As_windows_1254() = default;

    Interpret_As_windows_1254(const Interpret_As_windows_1254&) = delete;
    Interpret_As_windows_1254& operator=(const Interpret_As_windows_1254&) = delete;

    // Decoding function
    QString decodeWindows1254(const QByteArray& rawData);

    // Windows-1254 decoding table
    static const std::unordered_map<unsigned char, QChar> windows1254Table;
};
