#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_windows_949 {
public:
    // Singleton instance
    static Interpret_As_windows_949& instance();

    // Execute decoding for the provided editor
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_windows_949();
    ~Interpret_As_windows_949() = default;

    Interpret_As_windows_949(const Interpret_As_windows_949&) = delete;
    Interpret_As_windows_949& operator=(const Interpret_As_windows_949&) = delete;

    // Decoding function
    QString decodeWindows949(const QByteArray& rawData);

    // Windows-949 decoding table
    static const std::unordered_map<QByteArray, QChar> windows949Table;
};
