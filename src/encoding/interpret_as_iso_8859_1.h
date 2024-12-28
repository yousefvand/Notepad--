#pragma once

#include <QString>
#include <QPlainTextEdit>

class Interpret_As_ISO_8859_1 {
public:
    static Interpret_As_ISO_8859_1& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_ISO_8859_1() = default;
    ~Interpret_As_ISO_8859_1() = default;
    Interpret_As_ISO_8859_1(const Interpret_As_ISO_8859_1&) = delete;
    Interpret_As_ISO_8859_1& operator=(const Interpret_As_ISO_8859_1&) = delete;

    QString decodeISO88591(const QByteArray& iso88591Data);
};
