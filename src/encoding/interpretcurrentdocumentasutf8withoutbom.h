#pragma once

#include <QString>

class CodeEditor; // Forward declaration

class InterpretCurrentDocumentAsUTF8WithoutBOM
{
public:
    // Singleton access method
    static InterpretCurrentDocumentAsUTF8WithoutBOM& instance();

    // Method to interpret the current document as UTF-8 without BOM
    void execute(CodeEditor* editor);

private:
    // Private constructor for singleton pattern
    InterpretCurrentDocumentAsUTF8WithoutBOM() = default;
    ~InterpretCurrentDocumentAsUTF8WithoutBOM() = default;

    // Delete copy constructor and assignment operator
    InterpretCurrentDocumentAsUTF8WithoutBOM(const InterpretCurrentDocumentAsUTF8WithoutBOM&) = delete;
    InterpretCurrentDocumentAsUTF8WithoutBOM& operator=(const InterpretCurrentDocumentAsUTF8WithoutBOM&) = delete;
};
