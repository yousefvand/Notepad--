#pragma once

#include "searchoptions.h"
#include "../codeeditor.h"
#include <QTextEdit>

class Search {
public:
    // Constructor and Destructor
    explicit Search(CodeEditor* editor);
    virtual ~Search();

    // Virtual functions for searching with default implementations
    virtual bool findNext();            // Forward search placeholder
    virtual bool findPrevious();        // Reverse search placeholder

    // Virtual methods for managing search options and state
    virtual void setSearchOptions(const SearchOptions &options);
    virtual void highlightSearchResult();
    virtual void resetSearchState();
    virtual void selectAll();

protected:
    SearchOptions* m_options;           // Search options (keyword, method, etc.)
    CodeEditor* m_editor;                // Pointer to the QTextEdit (text editor)
    int m_currentPosition = 0;          // Current position in the text document
    bool m_isSearchInProgress = false;  // Flag to prevent overlapping search operations
};
