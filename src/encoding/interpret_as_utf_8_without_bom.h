#pragma once

#include <QString>

class CodeEditor; // Forward declaration

class InterpretAsUTF8WithoutBOM
{
public:
    // Singleton access method
    static InterpretAsUTF8WithoutBOM& instance();

    // Method to interpret the current document as UTF-8 without BOM
    void execute(CodeEditor* editor);

private:
    // Private constructor for singleton pattern
    InterpretAsUTF8WithoutBOM() = default;
    ~InterpretAsUTF8WithoutBOM() = default;

    // Delete copy constructor and assignment operator
    InterpretAsUTF8WithoutBOM(const InterpretAsUTF8WithoutBOM&) = delete;
    InterpretAsUTF8WithoutBOM& operator=(const InterpretAsUTF8WithoutBOM&) = delete;
};
