#pragma once

#include <QString>

class CodeEditor; // Forward declaration

class InterpretCurrentDocumentAsUTF8
{
public:
    // Singleton access
    static InterpretCurrentDocumentAsUTF8& instance();

    // Method to reinterpret the content of a CodeEditor as UTF-8
    void execute(CodeEditor* editor);

private:
    InterpretCurrentDocumentAsUTF8() = default; // Private constructor for singleton
    ~InterpretCurrentDocumentAsUTF8() = default; // Destructor
    InterpretCurrentDocumentAsUTF8(const InterpretCurrentDocumentAsUTF8&) = delete; // No copy
    InterpretCurrentDocumentAsUTF8& operator=(const InterpretCurrentDocumentAsUTF8&) = delete; // No assignment
};


