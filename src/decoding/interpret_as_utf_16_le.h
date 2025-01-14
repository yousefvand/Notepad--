#pragma once

#include <QString>

class CodeEditor;

class InterpretAsUTF16LE
{
public:
    static InterpretAsUTF16LE& instance();
    void execute(CodeEditor* editor);

private:
    InterpretAsUTF16LE(); // Private constructor
    ~InterpretAsUTF16LE();
    InterpretAsUTF16LE(const InterpretAsUTF16LE&) = delete;
    InterpretAsUTF16LE& operator=(const InterpretAsUTF16LE&) = delete;
};

