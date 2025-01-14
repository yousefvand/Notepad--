#pragma once

#include <QString>
#include <QPlainTextEdit>

class Interpret_As_ISO_8859_10 {
public:
    static Interpret_As_ISO_8859_10& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_ISO_8859_10() = default;
    ~Interpret_As_ISO_8859_10() = default;
    Interpret_As_ISO_8859_10(const Interpret_As_ISO_8859_10&) = delete;
    Interpret_As_ISO_8859_10& operator=(const Interpret_As_ISO_8859_10&) = delete;

    QString decodeISO885910(const QByteArray& iso885910Data);
};
