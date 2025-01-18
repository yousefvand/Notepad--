#pragma once

#include <QString>
#include <QPlainTextEdit>

class Interpret_As_ISO_8859_2 {
public:
    static Interpret_As_ISO_8859_2& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_ISO_8859_2() = default;
    ~Interpret_As_ISO_8859_2() = default;
    Interpret_As_ISO_8859_2(const Interpret_As_ISO_8859_2&) = delete;
    Interpret_As_ISO_8859_2& operator=(const Interpret_As_ISO_8859_2&) = delete;

    QString decodeISO88592(const QByteArray& iso88592Data);
};
