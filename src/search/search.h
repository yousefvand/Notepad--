#pragma once

#include <QTextEdit>
#include "searchoptions.h"
#include "../codeeditor.h"

class Search {
public:
    explicit Search(CodeEditor* editor = nullptr, SearchOptions* options = nullptr);
    virtual ~Search() = default;

    void setSearchOptions(const SearchOptions& options);
    void setEditor(CodeEditor* editor);  // Add setEditor declaration

    virtual bool findNext();
    virtual bool findPrevious();
    virtual void selectAll();
    virtual void replaceAll();

protected:
    bool search();
    void clearHighlights();

    CodeEditor* m_editor;
    QTextCursor m_cursor;
    SearchOptions* m_searchOptions;

private:
    QString expandSpecialCharacters(const QString& input) const;
};
