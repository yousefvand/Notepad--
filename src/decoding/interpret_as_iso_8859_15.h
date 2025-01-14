#pragma once

#include <QString>
#include <QPlainTextEdit>

class Interpret_As_ISO_8859_15 {
public:
    static Interpret_As_ISO_8859_15& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_ISO_8859_15() = default;
    ~Interpret_As_ISO_8859_15() = default;
    Interpret_As_ISO_8859_15(const Interpret_As_ISO_8859_15&) = delete;
    Interpret_As_ISO_8859_15& operator=(const Interpret_As_ISO_8859_15&) = delete;

    QString decodeISO885915(const QByteArray& iso885915Data);
};
