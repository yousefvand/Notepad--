#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>

class Interpret_As_Big5_HKSCS {
public:
    // Singleton instance access
    static Interpret_As_Big5_HKSCS& instance();

    // Execute decoding for the provided editor
    void execute(QPlainTextEdit* editor);

private:
    // Private constructor for Singleton pattern
    Interpret_As_Big5_HKSCS();
    ~Interpret_As_Big5_HKSCS() = default;

    // Disable copy construction and assignment
    Interpret_As_Big5_HKSCS(const Interpret_As_Big5_HKSCS&) = delete;
    Interpret_As_Big5_HKSCS& operator=(const Interpret_As_Big5_HKSCS&) = delete;

    // Decoding method for Big5-HKSCS data
    QString decodeBig5HKSCS(const QByteArray& rawData);
};
