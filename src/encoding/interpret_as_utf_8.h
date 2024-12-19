#pragma once

#include <QString>

class CodeEditor;

class InterpreteAsUtf8
{
public:
    static InterpreteAsUtf8& instance();
    void execute(CodeEditor* editor);

private:
    InterpreteAsUtf8() = default; // Singleton: private constructor
    ~InterpreteAsUtf8() = default;
};
