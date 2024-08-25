#ifndef LANGUAGEMANAGER_H
#define LANGUAGEMANAGER_H

#include <QString>
#include <QSyntaxHighlighter>
#include <QTextDocument>

class LanguageManager {
public:
    static QSyntaxHighlighter* createHighlighterForExtension(const QString &extension, QTextDocument *document);
    static QString getLanguageFromExtension(const QString &extension);
};

#endif // LANGUAGEMANAGER_H
