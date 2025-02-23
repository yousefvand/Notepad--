#pragma once

#include <QString>

class CodeEditor;

class Convert_To_Utf_8
{
public:
    static Convert_To_Utf_8& instance();
    void execute(CodeEditor* editor);

private:
    Convert_To_Utf_8() = default; // Singleton: private constructor
    ~Convert_To_Utf_8() = default;
};
