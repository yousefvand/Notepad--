#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>

class Interpret_As_GBK {
public:
    // Singleton instance access
    static Interpret_As_GBK& instance();

    // Execute decoding for the provided editor
    void execute(QPlainTextEdit* editor);

private:
    // Private constructor for Singleton pattern
    Interpret_As_GBK();
    ~Interpret_As_GBK() = default;

    // Disable copy construction and assignment
    Interpret_As_GBK(const Interpret_As_GBK&) = delete;
    Interpret_As_GBK& operator=(const Interpret_As_GBK&) = delete;

    // Decoding method for GBK data
    QString decodeGBK(const QByteArray& rawData);
};
