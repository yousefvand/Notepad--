#pragma once

#include <QString>
#include <QPlainTextEdit>

class Interpret_As_ISO_2022_KR {
public:
    static Interpret_As_ISO_2022_KR& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_ISO_2022_KR() = default;
    ~Interpret_As_ISO_2022_KR() = default;
    Interpret_As_ISO_2022_KR(const Interpret_As_ISO_2022_KR&) = delete;
    Interpret_As_ISO_2022_KR& operator=(const Interpret_As_ISO_2022_KR&) = delete;

    QString decodeISO2022KR(const QByteArray& iso2022krData);
    QString decodeEscapeSequence(const QByteArray& data);
};
