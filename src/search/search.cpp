#include <QDebug>
#include "search.h"
#include "../search/searchoptions.h"

Search::Search(CodeEditor* editor)
    : m_editor(editor), m_cursor(editor ? editor->document() : nullptr) {}

Search::~Search() = default;

void Search::setEditor(CodeEditor* editor) {
    m_editor = editor;
    m_cursor = m_editor ? QTextCursor(m_editor->document()) : QTextCursor();
}

void Search::setSearchOptions(const SearchOptions& options) {
    m_options = options;
}

bool Search::findNext() {
    if (!m_editor) {
        qWarning() << "Error: m_editor is null!";
        return false;
    }

    QTextDocument::FindFlags flags;
    if (m_options.matchCase) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    if (m_options.matchWholeWord) {
        flags |= QTextDocument::FindWholeWords;
    }

    QString keyword = m_options.keyword;
    m_cursor = m_editor->document()->find(keyword, m_cursor, flags);

    bool found = !m_cursor.isNull();
    if (found) {
        m_editor->setTextCursor(m_cursor);
    } else {
        qDebug() << "No further occurrences found.";
    }

    return found;
}

bool Search::findPrevious() {
    if (!m_editor) {
        qWarning() << "Error: m_editor is null!";
        return false;
    }

    QTextDocument::FindFlags flags = QTextDocument::FindBackward;
    if (m_options.matchCase) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    if (m_options.matchWholeWord) {
        flags |= QTextDocument::FindWholeWords;
    }

    QString keyword = m_options.keyword;
    m_cursor = m_editor->document()->find(keyword, m_cursor, flags);

    bool found = !m_cursor.isNull();
    if (found) {
        m_editor->setTextCursor(m_cursor);
    } else {
        qDebug() << "No further occurrences found.";
    }

    return found;
}

QString Search::expandSpecialCharacters(const QString& input) const {
    QString result = input;
    result.replace("\\n", "\n");
    result.replace("\\r", "\r");
    result.replace("\\t", "\t");
    result.replace("\\\\", "\\");
    result.replace("\\0", "\0");
    qDebug() << "Expanded search keyword:" << result;
    return result;
}

