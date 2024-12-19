#pragma once

#include <QString>
#include <QPlainTextEdit>

class InterpreteAsUTF32LE
{
public:
    static InterpreteAsUTF32LE& instance();

    void execute(QPlainTextEdit* editor);

private:
    InterpreteAsUTF32LE();
    ~InterpreteAsUTF32LE();

    InterpreteAsUTF32LE(const InterpreteAsUTF32LE&) = delete;
    InterpreteAsUTF32LE& operator=(const InterpreteAsUTF32LE&) = delete;

    QString fromUTF32LE(const QByteArray& data);
};

