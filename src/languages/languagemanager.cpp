#include "languagemanager.h"
#include "cppsyntaxhighlighter.h"
#include "pythonsyntaxhighlighter.h"

QSyntaxHighlighter* LanguageManager::createHighlighterForExtension(const QString &identifier, QTextDocument *document) {
    qDebug() << "Creating highlighter for language:" << identifier << "with document:" << document;
    if (document == nullptr) {
        qDebug() << "Error: document is nullptr";
        return nullptr;
    }

    // Create highlighter based on language identifier
    if (identifier == "C++") {
        auto* highlighter = new CppSyntaxHighlighter(document);
        qDebug() << "Created C++ syntax highlighter at:" << highlighter;
        return highlighter;
    }

    // Return nullptr if no matching highlighter is found
    qDebug() << "Error: No matching highlighter found for language:" << identifier;
    return nullptr;
}

QString LanguageManager::getLanguageFromExtension(const QString &extension) {
    if (extension == "cpp" || extension == "cxx" || extension == "h" || extension == "hpp") {
        return "C++";
    } else if (extension == "py") {
        return "Python";
    }
    return "Unknown";
}
