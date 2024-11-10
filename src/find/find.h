#pragma once

#include <QString>
#include <QTextEdit>
#include "../codeeditor.h"
#include "../search/searchoptions.h"

class Find {
public:
    explicit Find(CodeEditor* editor = nullptr);

    void setEditor(CodeEditor* editor);
    void setSearchOptions(const SearchOptions& options);

    bool findNext();           // Search forward
    bool findPrevious();       // Search backward
    void selectAll();          // Highlight all occurrences of the keyword
    bool searchAcrossAllTabs(bool backward);
    bool iterateOverTabs(bool backward);

private:
    QString expandSpecialCharacters(const QString& input) const;
    bool performSearch(bool backward);
    QTextDocument::FindFlags buildFlags(bool backward) const;
    bool search(bool backward);

    CodeEditor* m_editor;
    QTextCursor m_cursor;
    SearchOptions m_options;
};
