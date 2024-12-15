#pragma once

#include <QString>

class CodeEditor; // Forward declaration

class InterpretCurrentDocumentAsUTF16BE
{
public:
    // Singleton instance access
    static InterpretCurrentDocumentAsUTF16BE& instance();

    // Method to interpret the current document as UTF-16BE
    void execute(CodeEditor* editor);

private:
    // Private constructor for singleton
    InterpretCurrentDocumentAsUTF16BE() = default;
    ~InterpretCurrentDocumentAsUTF16BE() = default;

    // Delete copy constructor and assignment operator
    InterpretCurrentDocumentAsUTF16BE(const InterpretCurrentDocumentAsUTF16BE&) = delete;
    InterpretCurrentDocumentAsUTF16BE& operator=(const InterpretCurrentDocumentAsUTF16BE&) = delete;
};

