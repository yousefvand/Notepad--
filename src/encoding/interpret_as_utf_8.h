#pragma once

#include <QString>

class CodeEditor;

class Interpret_As_Utf_8
{
public:
    static Interpret_As_Utf_8& instance();
    void execute(CodeEditor* editor);

private:
    Interpret_As_Utf_8() = default; // Singleton: private constructor
    ~Interpret_As_Utf_8() = default;
};
