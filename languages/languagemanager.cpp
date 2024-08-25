#include "languagemanager.h"
#include "cppsyntaxhighlighter.h"
#include "pythonsyntaxhighlighter.h"

QSyntaxHighlighter* LanguageManager::createHighlighterForExtension(const QString &identifier, QTextDocument *document) {
    if (identifier == "C++" || identifier == "cpp" || identifier == "cxx" || identifier == "h" || identifier == "hpp") {
        return new CppSyntaxHighlighter(document);
    } else if (identifier == "Python" || identifier == "py") {
        return new PythonSyntaxHighlighter(document);
    }

    // Add more language mappings here as needed

    return nullptr;  // Return nullptr if no matching highlighter is found
}

QString LanguageManager::getLanguageFromExtension(const QString &extension) {
    if (extension == "cpp" || extension == "cxx" || extension == "h" || extension == "hpp") {
        return "C++";
    } else if (extension == "py") {
        return "Python";
    }
    return "Unknown";
}
