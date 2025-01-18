#pragma once

#include <QString>
#include <QByteArray>
#include <QPlainTextEdit>

class Interpret_As_ISO_8859_8 {
public:
    static Interpret_As_ISO_8859_8& instance();

    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_ISO_8859_8() = default;
    ~Interpret_As_ISO_8859_8() = default;

    Interpret_As_ISO_8859_8(const Interpret_As_ISO_8859_8&) = delete;
    Interpret_As_ISO_8859_8& operator=(const Interpret_As_ISO_8859_8&) = delete;

    QString decodeISO88598(const QByteArray& iso88598Data);
};
