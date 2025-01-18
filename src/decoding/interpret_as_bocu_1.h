#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>

class Interpret_As_BOCU_1 {
public:
    // Singleton instance access
    static Interpret_As_BOCU_1& instance();

    // Execute decoding for the provided editor
    void execute(QPlainTextEdit* editor);

private:
    // Private constructor for Singleton pattern
    Interpret_As_BOCU_1();
    ~Interpret_As_BOCU_1() = default;

    // Disable copy construction and assignment
    Interpret_As_BOCU_1(const Interpret_As_BOCU_1&) = delete;
    Interpret_As_BOCU_1& operator=(const Interpret_As_BOCU_1&) = delete;

    // Decoding method for BOCU-1 data
    QString decodeBOCU1(const QByteArray& rawData);

    // Helper to decode a single BOCU-1 byte sequence
    int decodeBOCU1Char(const QByteArray& data, int& index);
};
