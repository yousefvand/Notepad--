#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>

class Interpret_As_HZ_GB_2312 {
public:
    // Singleton instance access
    static Interpret_As_HZ_GB_2312& instance();

    // Execute decoding for the provided editor
    void execute(QPlainTextEdit* editor);

private:
    // Private constructor for Singleton pattern
    Interpret_As_HZ_GB_2312();
    ~Interpret_As_HZ_GB_2312() = default;

    // Disable copy construction and assignment
    Interpret_As_HZ_GB_2312(const Interpret_As_HZ_GB_2312&) = delete;
    Interpret_As_HZ_GB_2312& operator=(const Interpret_As_HZ_GB_2312&) = delete;

    // Decoding method for HZ-GB-2312 data
    QString decodeHZGB2312(const QByteArray& rawData);
};
