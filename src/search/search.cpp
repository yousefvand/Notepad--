#include "search.h"
#include <QMessageBox>

Search::Search(CodeEditor* editor)
    : m_options(new SearchOptions()), m_editor(editor) {
    // Any additional initialization if needed
}

Search::~Search() {
    delete m_options;  // Clean up the allocated memory for m_options
}

bool Search::findNext() {
    // Placeholder implementation for forward search
    if (!m_editor || m_options->keyword.isEmpty()) {
        QMessageBox::information(nullptr, "Search", "No search keyword specified.");
        return false;
    }

    // Basic implementation example, just to illustrate a forward search structure
    QTextDocument::FindFlags flags;
    if (m_options->matchCase) flags |= QTextDocument::FindCaseSensitively;
    if (m_options->matchWholeWord) flags |= QTextDocument::FindWholeWords;

    bool found = m_editor->find(m_options->keyword, flags);
    if (!found) {
        QMessageBox::information(nullptr, "Search", "No further occurrences found.");
    }
    return found;
}

bool Search::findPrevious() {
    // Placeholder implementation for reverse search
    if (!m_editor || m_options->keyword.isEmpty()) {
        QMessageBox::information(nullptr, "Search", "No search keyword specified.");
        return false;
    }

    // Basic implementation example for a reverse search
    QTextDocument::FindFlags flags = QTextDocument::FindBackward;
    if (m_options->matchCase) flags |= QTextDocument::FindCaseSensitively;
    if (m_options->matchWholeWord) flags |= QTextDocument::FindWholeWords;

    bool found = m_editor->find(m_options->keyword, flags);
    if (!found) {
        QMessageBox::information(nullptr, "Search", "No previous occurrences found.");
    }
    return found;
}

void Search::setSearchOptions(const SearchOptions &options) {
    *m_options = options;
}

void Search::highlightSearchResult() {
    // Placeholder: To be implemented in derived classes
}

void Search::resetSearchState() {
    m_currentPosition = 0;
    m_isSearchInProgress = false;
}

void Search::selectAll() {
    // Placeholder: To be implemented in derived classes
}
