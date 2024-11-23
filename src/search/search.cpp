#include <QDebug>
#include <QRegularExpression>
#include "search.h"
#include "../search/searchoptions.h"

Search::Search(CodeEditor* editor, SearchOptions* options)
    : m_editor(editor)
    , m_cursor(editor ? editor->document() : nullptr)
    , m_searchOptions(new SearchOptions()) {

    if (options) m_searchOptions = options;
}

bool Search::findNext() {
    m_searchOptions->role = Role::FindNext;
    setSearchOptions(*m_searchOptions);
    return search();
}

bool Search::findPrevious() {
    m_searchOptions->role = Role::FindPrevious;
    setSearchOptions(*m_searchOptions);
    return search();
}

void Search::selectAll() {
    m_searchOptions->role = Role::SelectAll;
    setSearchOptions(*m_searchOptions);

    if (!m_editor || !m_searchOptions || !m_editor->document()) {
        return; // Ensure the editor, options, and document are valid
    }

    // Check the role (must be SelectAll)
    if (m_searchOptions->role != Role::SelectAll) {
        return; // Exit if the role is not SelectAll
    }

    QTextDocument* document = m_editor->document();
    QTextCursor cursor(document);

    // Prepare the keyword or regex pattern
    QString pattern;
    if (m_searchOptions->findMethod == FindMethod::RegularExpression) {
        pattern = m_searchOptions->keyword;
    } else if (m_searchOptions->findMethod == FindMethod::SpecialCharacters) {
        // Handle special characters like \n, \t, \r
        pattern = QRegularExpression::escape(m_searchOptions->keyword)
                      .replace("\\n", "\n").replace("\\t", "\t")
                      .replace("\\r", "\r").replace("\\0", "\0").replace("\\\\", "\\");
    } else { // Simple text search
        pattern = QRegularExpression::escape(m_searchOptions->keyword);
    }

    // Add whole word boundaries if needed
    if (m_searchOptions->matchWholeWord) {
        pattern = "\\b" + pattern + "\\b";
    }

    // Compile the regular expression
    QRegularExpression regex(pattern);
    if (!regex.isValid()) {
        return; // Invalid regex, do nothing
    }

    // Case sensitivity
    QRegularExpression::PatternOptions regexOptions = QRegularExpression::NoPatternOption;
    if (!m_searchOptions->matchCase) {
        regexOptions |= QRegularExpression::CaseInsensitiveOption;
    }
    regex.setPatternOptions(regexOptions);

    // Highlighting format
    QTextCharFormat highlightFormat;
    highlightFormat.setBackground(Qt::yellow);
    highlightFormat.setForeground(Qt::black);

    // Start iterating over the document to find and highlight matches
    cursor.movePosition(QTextCursor::Start); // Start from the beginning of the document
    while (!cursor.isNull() && !cursor.atEnd()) {
        cursor = document->find(regex, cursor); // Find the next match
        if (!cursor.isNull()) {
            cursor.mergeCharFormat(highlightFormat); // Apply highlight to matched text
        }
    }
}

// FIXME: replaceAll causes segmentation fault.
void Search::replaceAll() {
    m_searchOptions->role = Role::ReplaceAll;
    setSearchOptions(*m_searchOptions);

    if (!m_editor || !m_searchOptions || !m_editor->document()) {
        qDebug() << "Editor, search options, or document is invalid.";
        return;
    }

    QTextDocument* document = m_editor->document();
    if (!document) {
        qDebug() << "Document is null.";
        return;
    }

    QSignalBlocker blocker(m_editor); // Block signals during the operation

    // Extract plain text from the document
    QString documentText = document->toPlainText();
    if (documentText.isEmpty()) {
        qDebug() << "Document is empty.";
        return;
    }

    // Prepare the regex pattern
    QString pattern;
    if (m_searchOptions->findMethod == FindMethod::RegularExpression) {
        pattern = m_searchOptions->keyword;
    } else if (m_searchOptions->findMethod == FindMethod::SpecialCharacters) {
        pattern = QRegularExpression::escape(m_searchOptions->keyword)
        .replace("\\n", "\n")
            .replace("\\t", "\t")
            .replace("\\r", "\r")
            .replace("\\0", "\0")
            .replace("\\\\", "\\");
    } else {
        pattern = QRegularExpression::escape(m_searchOptions->keyword);
    }

    if (m_searchOptions->matchWholeWord) {
        pattern = "\\b" + pattern + "\\b";
    }

    QRegularExpression regex(pattern);
    if (!regex.isValid()) {
        qDebug() << "Invalid regex pattern.";
        return;
    }

    QRegularExpression::PatternOptions regexOptions = QRegularExpression::NoPatternOption;
    if (!m_searchOptions->matchCase) {
        regexOptions |= QRegularExpression::CaseInsensitiveOption;
    }
    regex.setPatternOptions(regexOptions);

    QString replacement = m_searchOptions->replaceText;

    // Replace matches in memory
    QString updatedText = documentText;
    QRegularExpressionMatchIterator it = regex.globalMatch(documentText);
    int replaceCount = 0;

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        if (match.hasMatch()) {
            updatedText.replace(match.capturedStart(), match.capturedLength(), replacement);
            ++replaceCount;
        }
    }

    // Update the document with the modified text
    document->setPlainText(updatedText);

    qDebug() << "Replacement complete. Replaced" << replaceCount << "occurrences.";
}

void Search::setEditor(CodeEditor* editor) {
    m_editor = editor;
    m_cursor = m_editor ? QTextCursor(m_editor->document()) : QTextCursor();
}

void Search::setSearchOptions(const SearchOptions& options) {
    clearHighlights();
    *m_searchOptions = options;
}

bool Search::search() {
    if (!m_editor || !m_searchOptions || m_cursor.isNull()) {
        return false; // Ensure valid editor, cursor, and search options
    }

    QTextDocument::FindFlags flags;

    // Determine direction based on Role
    if (m_searchOptions->role == Role::FindPrevious || m_searchOptions->role == Role::ReplacePrevious) {
        flags |= QTextDocument::FindBackward; // Backward search for previous roles
    } else if (m_searchOptions->role == Role::FindNext || m_searchOptions->role == Role::ReplaceNext) {
        // No flag needed for forward search (default behavior)
    } else {
        return false; // Invalid role for search
    }

    if (m_searchOptions->matchCase) {
        flags |= QTextDocument::FindCaseSensitively; // Case sensitivity
    }

    // Prepare the keyword based on FindMethod
    QString keyword;
    if (m_searchOptions->findMethod == FindMethod::SpecialCharacters) {
        keyword = expandSpecialCharacters(m_searchOptions->keyword);
    } else if (m_searchOptions->findMethod == FindMethod::RegularExpression) {
        keyword = m_searchOptions->keyword; // Use directly for regex
    } else { // Simple text search
        keyword = QRegularExpression::escape(m_searchOptions->keyword);
    }

    if (m_searchOptions->matchWholeWord) {
        // Add whole word boundaries
        keyword = "\\b" + keyword + "\\b";
    }

    // Compile regex
    QRegularExpression regex(keyword);
    if (!regex.isValid()) {
        qDebug() << "Invalid regex in search: " << regex;
        return false; // Invalid regex, do nothing
    }
    regex.setPatternOptions(m_searchOptions->matchCase ? QRegularExpression::NoPatternOption
                                                       : QRegularExpression::CaseInsensitiveOption);

    // Perform the search
    QTextCursor resultCursor = m_editor->document()->find(regex, m_cursor, flags);

    if (!resultCursor.isNull()) {
        // Handle replacement if Role is ReplaceNext or ReplacePrevious
        if (m_searchOptions->role == Role::ReplaceNext || m_searchOptions->role == Role::ReplacePrevious) {
            if (!m_searchOptions->replaceText.isEmpty()) {
                resultCursor.insertText(m_searchOptions->replaceText);
            }
        } else if (m_searchOptions->role == Role::FindNext || m_searchOptions->role == Role::FindPrevious) {
            m_editor->setTextCursor(resultCursor); // Move the cursor for Find roles
        }

        // Update m_cursor for the next operation
        m_cursor = resultCursor;

        return true; // Match found (and optionally replaced)
    }

    return false; // No match found
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

void Search::clearHighlights() {
    if (!m_editor || !m_editor->document()) {
        return;
    }

    QTextDocument* document = m_editor->document();
    QTextCursor cursor(document);

    // Select the entire document
    cursor.select(QTextCursor::Document);

    // Create a default format with no background or text color
    QTextCharFormat defaultFormat;
    defaultFormat.clearBackground(); // Removes background color
    defaultFormat.clearForeground(); // Resets text color to default

    // Apply the default format to the entire document
    cursor.setCharFormat(defaultFormat);
}





