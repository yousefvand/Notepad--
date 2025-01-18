#pragma once

#include <QString>

class CodeEditor; // Forward declaration

class Interpret_As_UTF_16_BE
{
public:
    // Singleton instance access
    static Interpret_As_UTF_16_BE& instance();

    // Method to interpret the current document as UTF-16BE
    void execute(CodeEditor* editor);

private:
    // Private constructor for singleton
    Interpret_As_UTF_16_BE() = default;
    ~Interpret_As_UTF_16_BE() = default;

    // Delete copy constructor and assignment operator
    Interpret_As_UTF_16_BE(const Interpret_As_UTF_16_BE&) = delete;
    Interpret_As_UTF_16_BE& operator=(const Interpret_As_UTF_16_BE&) = delete;
};

