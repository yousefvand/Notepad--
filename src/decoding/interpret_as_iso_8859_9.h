#pragma once

#include <QString>
#include <QByteArray>
#include <QPlainTextEdit>

class Interpret_As_ISO_8859_9 {
public:
    static Interpret_As_ISO_8859_9& instance();

    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_ISO_8859_9() = default;
    ~Interpret_As_ISO_8859_9() = default;

    Interpret_As_ISO_8859_9(const Interpret_As_ISO_8859_9&) = delete;
    Interpret_As_ISO_8859_9& operator=(const Interpret_As_ISO_8859_9&) = delete;

    QString decodeISO88599(const QByteArray& iso88599Data);
};
