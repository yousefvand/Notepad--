#pragma once

#include <QString>
#include <QByteArray>
#include <QPlainTextEdit>

// Singleton class to interpret files as KOI8-R
class Interpret_As_KOI8_R {
public:
    static Interpret_As_KOI8_R& instance();

    // Main decoding method
    QString decodeKOI8R(const QByteArray& koi8Data);

    // Executes KOI8-R interpretation for the given editor
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_KOI8_R() = default;
    ~Interpret_As_KOI8_R() = default;

    Interpret_As_KOI8_R(const Interpret_As_KOI8_R&) = delete;
    Interpret_As_KOI8_R& operator=(const Interpret_As_KOI8_R&) = delete;
};
