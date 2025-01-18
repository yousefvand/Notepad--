#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>

class Interpret_As_Big5 {
public:
    // Singleton instance access
    static Interpret_As_Big5& instance();

    // Execute decoding for the provided editor
    void execute(QPlainTextEdit* editor);

private:
    // Private constructor for Singleton pattern
    Interpret_As_Big5();
    ~Interpret_As_Big5() = default;

    // Disable copy construction and assignment
    Interpret_As_Big5(const Interpret_As_Big5&) = delete;
    Interpret_As_Big5& operator=(const Interpret_As_Big5&) = delete;

    // Decoding method for Big5 data
    QString decodeBig5(const QByteArray& rawData);
};
