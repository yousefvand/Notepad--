#pragma once

#include <QString>
#include <QByteArray>
#include <QPlainTextEdit>

class Interpret_As_ISO_8859_7 {
public:
    static Interpret_As_ISO_8859_7& instance();

    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_ISO_8859_7() = default;
    ~Interpret_As_ISO_8859_7() = default;

    Interpret_As_ISO_8859_7(const Interpret_As_ISO_8859_7&) = delete;
    Interpret_As_ISO_8859_7& operator=(const Interpret_As_ISO_8859_7&) = delete;

    QString decodeISO88597(const QByteArray& iso88597Data);
};
