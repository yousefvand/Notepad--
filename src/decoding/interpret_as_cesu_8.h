#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>

class Interpret_As_CESU_8 {
public:
    // Singleton instance access
    static Interpret_As_CESU_8& instance();

    // Execute decoding for the provided editor
    void execute(QPlainTextEdit* editor);

private:
    // Private constructor for Singleton pattern
    Interpret_As_CESU_8();
    ~Interpret_As_CESU_8() = default;

    // Disable copy construction and assignment
    Interpret_As_CESU_8(const Interpret_As_CESU_8&) = delete;
    Interpret_As_CESU_8& operator=(const Interpret_As_CESU_8&) = delete;

    // Decoding method for CESU-8 data
    QString decodeCESU8(const QByteArray& rawData);
};
