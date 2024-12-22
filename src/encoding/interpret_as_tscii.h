#pragma once

#include <QPlainTextEdit>

class Interpret_As_TSCII {
public:
    static Interpret_As_TSCII& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_TSCII() = default;
    ~Interpret_As_TSCII() = default;

    Interpret_As_TSCII(const Interpret_As_TSCII&) = delete;
    Interpret_As_TSCII& operator=(const Interpret_As_TSCII&) = delete;

    QString decodeTSCII(const QByteArray& tsciiData);
    static const QMap<uint8_t, QChar> tsciiToUnicode;
};
