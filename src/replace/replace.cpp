#include "replace.h"

Replace::Replace(CodeEditor* editor)
    : Search(editor) {}

bool Replace::replaceNext() {
    if (findNext()) {
        m_cursor.insertText(m_options.replaceText);  // Replace the found text
        return true;
    }
    return false;
}

bool Replace::replacePrevious() {
    if (findPrevious()) {
        m_cursor.insertText(m_options.replaceText);  // Replace the found text
        return true;
    }
    return false;
}

void Replace::replaceAll() {
    m_cursor.movePosition(QTextCursor::Start);
    while (replaceNext()) {
        // Loop to replace all occurrences
    }
}

void Replace::selectAll() {
    if (!m_editor) {
        qWarning() << "Error: m_editor is null!";
        return;
    }

    QTextDocument *document = m_editor->document();
    QTextCursor cursor(document);
    cursor.movePosition(QTextCursor::Start);
    QString keyword = expandSpecialCharacters(m_options.keyword);

    QTextDocument::FindFlags flags;
    if (m_options.matchCase) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    if (m_options.matchWholeWord) {
        flags |= QTextDocument::FindWholeWords;
    }

    QList<QTextEdit::ExtraSelection> extraSelections;

    while (!cursor.isNull() && !cursor.atEnd()) {
        cursor = document->find(keyword, cursor, flags);
        if (!cursor.isNull()) {
            QTextEdit::ExtraSelection selection;
            selection.cursor = cursor;
            selection.format.setBackground(Qt::yellow); // Highlight color
            extraSelections.append(selection);
        }
    }

    m_editor->setExtraSelections(extraSelections);
}

