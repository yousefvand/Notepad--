#pragma once

#include <QString>
#include <QVector>
#include <QPair>

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
    QString location;              // The path to look for files
    QString pattern;               // The comma deliminated file patterns by * and ?
    FindMethod findMethod;         // The search method (simple text, regex, special characters)
    bool matchWholeWord;           // Flag to indicate if we match the whole word
    bool matchCase;                // Flag to indicate case sensitivity
    bool allTabs;                  // Flag to search through all open tabs
    bool includeSubdirectories;    // Flag to search through all subdirectories

    // Default constructor
    SearchOptions()
        : keyword(""), replaceText(""), location(""), pattern(""), findMethod(FindMethod::SimpleText),
          matchWholeWord(false), matchCase(false), allTabs(false), includeSubdirectories(false) {}
};

struct SearchResult {
    QString filePath;                   // Path of the file containing matches
    QVector<QPair<int, int>> matches;   // List of pairs indicating start and end positions of each match
};

struct SearchProgress {
    int processedFiles = 0;             // Number of files processed
    int totalFiles = 0;                 // Total files to process
};

struct FileSearchResults {
    QString filePath;                           // Path of the file
    int matchCount;                             // Number of matches in the file
    QVector<QString> matchingLines;             // Lines containing matches, with keywords highlighted
    QVector<QPair<int, QString>> matches;       // Pairs of (line number, line text) for each match
};

