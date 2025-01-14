#pragma once

#include <QString>
#include <QPlainTextEdit>

class Interpret_As_ISO_8859_5 {
public:
    static Interpret_As_ISO_8859_5& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_ISO_8859_5() = default;
    ~Interpret_As_ISO_8859_5() = default;
    Interpret_As_ISO_8859_5(const Interpret_As_ISO_8859_5&) = delete;
    Interpret_As_ISO_8859_5& operator=(const Interpret_As_ISO_8859_5&) = delete;

    QString decodeISO88595(const QByteArray& iso88595Data);
};
