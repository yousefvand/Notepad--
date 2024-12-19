#pragma once

#include <QString>
#include <QPlainTextEdit>
#include <QFile>
#include <QTextStream>
#include <QDebug>

class InterpretAsUtf32 {
public:
    static InterpretAsUtf32& instance();

    void execute(QPlainTextEdit* editor);

private:
    InterpretAsUtf32() = default;
    ~InterpretAsUtf32() = default;

    InterpretAsUtf32(const InterpretAsUtf32&) = delete;
    InterpretAsUtf32& operator=(const InterpretAsUtf32&) = delete;
};
