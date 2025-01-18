#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_cp851 {
public:
    // Singleton instance access
    static Interpret_As_cp851& instance();

    // Execute decoding for the provided editor
    void execute(QPlainTextEdit* editor);

private:
    // Private constructor for Singleton pattern
    Interpret_As_cp851();
    ~Interpret_As_cp851() = default;

    // Disable copy construction and assignment
    Interpret_As_cp851(const Interpret_As_cp851&) = delete;
    Interpret_As_cp851& operator=(const Interpret_As_cp851&) = delete;

    // Decoding method for CP851 data
    QString decodeCP851(const QByteArray& rawData);

    // CP851 encoding table
    static const std::unordered_map<unsigned char, QChar> cp851Table;
};
