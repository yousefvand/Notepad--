#pragma once

#include <QString>
#include <QPlainTextEdit>

class Interpret_As_ISO_8859_3 {
public:
    static Interpret_As_ISO_8859_3& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_ISO_8859_3() = default;
    ~Interpret_As_ISO_8859_3() = default;
    Interpret_As_ISO_8859_3(const Interpret_As_ISO_8859_3&) = delete;
    Interpret_As_ISO_8859_3& operator=(const Interpret_As_ISO_8859_3&) = delete;

    QString decodeISO88593(const QByteArray& iso88593Data);
};
