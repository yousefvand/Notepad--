#pragma once

#include <QTextEdit>
#include "searchoptions.h"
#include "../codeeditor.h"

class Search {
public:
    explicit Search(CodeEditor* editor = nullptr);
    virtual ~Search();

    void setSearchOptions(const SearchOptions& options);
    void setEditor(CodeEditor* editor);  // Add setEditor declaration

    virtual bool findNext();
    virtual bool findPrevious();

protected:
    SearchOptions m_options;
    CodeEditor* m_editor;   // Editor pointer of type CodeEditor
    QTextCursor m_cursor;
    QString expandSpecialCharacters(const QString& input) const;
};
