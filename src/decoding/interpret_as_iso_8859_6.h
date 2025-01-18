#pragma once

#include <QString>
#include <QByteArray>
#include <QPlainTextEdit>

class Interpret_As_ISO_8859_6 {
public:
    static Interpret_As_ISO_8859_6& instance();

    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_ISO_8859_6() = default;
    ~Interpret_As_ISO_8859_6() = default;

    Interpret_As_ISO_8859_6(const Interpret_As_ISO_8859_6&) = delete;
    Interpret_As_ISO_8859_6& operator=(const Interpret_As_ISO_8859_6&) = delete;

    QString decodeISO88596(const QByteArray& iso88596Data);
};
