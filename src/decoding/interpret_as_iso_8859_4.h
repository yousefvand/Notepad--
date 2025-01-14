#pragma once

#include <QString>
#include <QPlainTextEdit>

class Interpret_As_ISO_8859_4 {
public:
    static Interpret_As_ISO_8859_4& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_ISO_8859_4() = default;
    ~Interpret_As_ISO_8859_4() = default;
    Interpret_As_ISO_8859_4(const Interpret_As_ISO_8859_4&) = delete;
    Interpret_As_ISO_8859_4& operator=(const Interpret_As_ISO_8859_4&) = delete;

    QString decodeISO88594(const QByteArray& iso88594Data);
};
