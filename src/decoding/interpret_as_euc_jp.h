#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>

class Interpret_As_EUC_JP {
public:
    // Singleton instance access
    static Interpret_As_EUC_JP& instance();

    // Execute decoding for the provided editor
    void execute(QPlainTextEdit* editor);

private:
    // Private constructor for Singleton pattern
    Interpret_As_EUC_JP();
    ~Interpret_As_EUC_JP() = default;

    // Disable copy construction and assignment
    Interpret_As_EUC_JP(const Interpret_As_EUC_JP&) = delete;
    Interpret_As_EUC_JP& operator=(const Interpret_As_EUC_JP&) = delete;

    // Decoding method for EUC-JP data
    QString decodeEUCJP(const QByteArray& rawData);
};
