#pragma once

#include <QString>
#include <QByteArray>
#include <QPlainTextEdit>

class Interpret_As_KOI8_U {
public:
    static Interpret_As_KOI8_U& instance();

    // Main decoding method
    QString decodeKOI8U(const QByteArray& koi8uData);

    // Executes the KOI8-U interpretation for a given editor
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_KOI8_U() = default;
    ~Interpret_As_KOI8_U() = default;

    Interpret_As_KOI8_U(const Interpret_As_KOI8_U&) = delete;
    Interpret_As_KOI8_U& operator=(const Interpret_As_KOI8_U&) = delete;

    QChar koi8uToUnicode(uint8_t koi8uChar);
};
