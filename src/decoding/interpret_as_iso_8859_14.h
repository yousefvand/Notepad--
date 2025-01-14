#pragma once

#include <QString>
#include <QPlainTextEdit>

class Interpret_As_ISO_8859_14 {
public:
    static Interpret_As_ISO_8859_14& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_ISO_8859_14() = default;
    ~Interpret_As_ISO_8859_14() = default;
    Interpret_As_ISO_8859_14(const Interpret_As_ISO_8859_14&) = delete;
    Interpret_As_ISO_8859_14& operator=(const Interpret_As_ISO_8859_14&) = delete;

    QString decodeISO885914(const QByteArray& iso885914Data);
};
