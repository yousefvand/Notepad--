#pragma once

#include <QString>
#include <QPlainTextEdit>

class Interpret_As_ISO_8859_13 {
public:
    static Interpret_As_ISO_8859_13& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_ISO_8859_13() = default;
    ~Interpret_As_ISO_8859_13() = default;
    Interpret_As_ISO_8859_13(const Interpret_As_ISO_8859_13&) = delete;
    Interpret_As_ISO_8859_13& operator=(const Interpret_As_ISO_8859_13&) = delete;

    QString decodeISO885913(const QByteArray& iso885913Data);
};
