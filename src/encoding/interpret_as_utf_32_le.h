#pragma once

#include <QString>
#include <QPlainTextEdit>
#include <QByteArray>

class Interpret_As_UTF_32_LE {
public:
    static Interpret_As_UTF_32_LE& instance();

    // Execute UTF-32LE interpretation for the given editor
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_UTF_32_LE() = default;
    ~Interpret_As_UTF_32_LE() = default;

    Interpret_As_UTF_32_LE(const Interpret_As_UTF_32_LE&) = delete;
    Interpret_As_UTF_32_LE& operator=(const Interpret_As_UTF_32_LE&) = delete;

    QString decodeUTF32LE(const QByteArray& utf32Data);
};
