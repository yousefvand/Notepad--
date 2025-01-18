#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_hp_roman8 {
public:
    // Singleton instance access
    static Interpret_As_hp_roman8& instance();

    // Execute decoding for the provided editor
    void execute(QPlainTextEdit* editor);

private:
    // Private constructor for Singleton pattern
    Interpret_As_hp_roman8();
    ~Interpret_As_hp_roman8() = default;

    // Disable copy construction and assignment
    Interpret_As_hp_roman8(const Interpret_As_hp_roman8&) = delete;
    Interpret_As_hp_roman8& operator=(const Interpret_As_hp_roman8&) = delete;

    // Decoding method for HP Roman8 data
    QString decodeHPRoman8(const QByteArray& rawData);

    // HP Roman8 encoding table
    static const std::unordered_map<unsigned char, QChar> hpRoman8Table;
};
