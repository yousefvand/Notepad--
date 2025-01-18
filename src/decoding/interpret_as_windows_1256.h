#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_windows_1256 {
public:
    // Singleton instance
    static Interpret_As_windows_1256& instance();

    // Execute decoding for the provided editor
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_windows_1256();
    ~Interpret_As_windows_1256() = default;

    Interpret_As_windows_1256(const Interpret_As_windows_1256&) = delete;
    Interpret_As_windows_1256& operator=(const Interpret_As_windows_1256&) = delete;

    // Decoding function
    QString decodeWindows1256(const QByteArray& rawData);

    // Windows-1256 decoding table
    static const std::unordered_map<unsigned char, QChar> windows1256Table;
};
