#pragma once

#include <QString>

class CodeEditor; // Forward declaration

class Interpret_As_UTF_8_Without_BOM
{
public:
    // Singleton access method
    static Interpret_As_UTF_8_Without_BOM& instance();

    // Method to interpret the current document as UTF-8 without BOM
    void execute(CodeEditor* editor);

private:
    // Private constructor for singleton pattern
    Interpret_As_UTF_8_Without_BOM() = default;
    ~Interpret_As_UTF_8_Without_BOM() = default;

    // Delete copy constructor and assignment operator
    Interpret_As_UTF_8_Without_BOM(const Interpret_As_UTF_8_Without_BOM&) = delete;
    Interpret_As_UTF_8_Without_BOM& operator=(const Interpret_As_UTF_8_Without_BOM&) = delete;
};
