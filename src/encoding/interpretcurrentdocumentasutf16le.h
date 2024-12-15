#pragma once

#include <QString>

class CodeEditor;

class InterpretCurrentDocumentAsUTF16LE
{
public:
    static InterpretCurrentDocumentAsUTF16LE& instance();
    void execute(CodeEditor* editor);

private:
    InterpretCurrentDocumentAsUTF16LE(); // Private constructor
    ~InterpretCurrentDocumentAsUTF16LE();
    InterpretCurrentDocumentAsUTF16LE(const InterpretCurrentDocumentAsUTF16LE&) = delete;
    InterpretCurrentDocumentAsUTF16LE& operator=(const InterpretCurrentDocumentAsUTF16LE&) = delete;
};

