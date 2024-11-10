#pragma once

#include <QString>

// Enum to represent the search method
enum class FindMethod {
    SimpleText,
    RegularExpression,
    SpecialCharacters
};

// Struct to hold search options
struct SearchOptions {
    QString keyword;               // The search keyword
    QString replaceText;           // Text to replace with
    FindMethod findMethod;         // The search method (simple text, regex, special characters)
    bool matchWholeWord;           // Flag to indicate if we match the whole word
    bool matchCase;                // Flag to indicate case sensitivity
    bool allTabs;                  // Flag to search through all open tabs

    // Default constructor
    SearchOptions()
        : keyword(""), findMethod(FindMethod::SimpleText), matchWholeWord(false),
        matchCase(false), allTabs(false) {}
};
