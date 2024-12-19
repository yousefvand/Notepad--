#pragma once

#include <QString>

class CodeEditor; // Forward declaration

class InterpretAsUTF16BE
{
public:
    // Singleton instance access
    static InterpretAsUTF16BE& instance();

    // Method to interpret the current document as UTF-16BE
    void execute(CodeEditor* editor);

private:
    // Private constructor for singleton
    InterpretAsUTF16BE() = default;
    ~InterpretAsUTF16BE() = default;

    // Delete copy constructor and assignment operator
    InterpretAsUTF16BE(const InterpretAsUTF16BE&) = delete;
    InterpretAsUTF16BE& operator=(const InterpretAsUTF16BE&) = delete;
};

