#include "find.h"
#include <QMessageBox>
#include <QTextCursor>
#include <QDebug>
#include <QTabWidget>
#include <QRegularExpression>
#include <QCoreApplication>

Find::Find(CodeEditor* editor)
    : m_editor(editor), m_cursor(editor ? editor->document() : nullptr) {}

void Find::setEditor(CodeEditor* editor) {
    m_editor = editor;
    m_cursor = m_editor ? QTextCursor(m_editor->document()) : QTextCursor();
}

void Find::setSearchOptions(const SearchOptions& options) {
    m_options = options;
}

QString Find::expandSpecialCharacters(const QString& input) const {
    QString result = input;

    result.replace("\\n", "\n");
    result.replace("\\r", "\r");
    result.replace("\\t", "\t");
    result.replace("\\\\", "\\");
    result.replace("\\0", "\0");

    qDebug() << "Expanded search keyword:" << result;
    return result;
}

QTextDocument::FindFlags Find::buildFlags(bool backward) const {
    QTextDocument::FindFlags flags;
    if (m_options.matchCase) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    if (m_options.matchWholeWord) {
        flags |= QTextDocument::FindWholeWords;
    }
    if (backward) {
        flags |= QTextDocument::FindBackward;
    }
    return flags;
}

bool Find::search(bool backward) {
    if (!m_editor) {
        qWarning() << "Error: m_editor is null!";
        return false;
    }

    QTextDocument::FindFlags flags = buildFlags(backward);
    QString keyword = expandSpecialCharacters(m_options.keyword);

    // Perform the search
    m_cursor = m_editor->document()->find(keyword, m_cursor, flags);

    bool found = !m_cursor.isNull();
    if (found) {
        m_editor->setTextCursor(m_cursor);
    }

    return found;
}

bool Find::findNext() {
    return search(false);
}

bool Find::findPrevious() {
    return search(true);
}

void Find::selectAll() {
    if (!m_editor) {
        qWarning() << "Error: m_editor is null!";
        return;
    }

    QTextDocument* document = m_editor->document();
    QTextCursor cursor(document);
    cursor.movePosition(QTextCursor::Start);
    QString keyword = expandSpecialCharacters(m_options.keyword);

    QTextDocument::FindFlags flags = buildFlags(false);
    QList<QTextEdit::ExtraSelection> extraSelections;

    while (true) {
        cursor = document->find(keyword, cursor, flags);

        if (cursor.isNull()) {
            break;
        }

        QTextEdit::ExtraSelection selection;
        selection.cursor = cursor;
        selection.format.setBackground(Qt::yellow);
        extraSelections.append(selection);
    }

    m_editor->setExtraSelections(extraSelections);
}

bool Find::iterateOverTabs(bool backward) {
    if (!m_editor) {
        qWarning() << "Error: m_editor is null!";
        return false;
    }

    // Get the tab widget (DocumentsTab) from the current editor
    QTabWidget* tabWidget = m_editor->DocumentsTab();
    if (!tabWidget) {
        qWarning() << "Error: No DocumentsTab available!";
        return false;
    }

    int tabCount = tabWidget->count();
    if (tabCount == 0) {
        qWarning() << "No tabs available for search!";
        return false;
    }

    int currentIndex = tabWidget->currentIndex();
    int startIndex = currentIndex; // Store the starting tab to avoid infinite loops

    qDebug() << "Starting search across all tabs. Total tabs:" << tabCount;

    do {
        qDebug() << "Switching to tab index:" << currentIndex;

        // Switch to the current tab
        tabWidget->setCurrentIndex(currentIndex);
        QCoreApplication::processEvents(); // Ensure the UI reflects the active tab change

        // Retrieve the CodeEditor for the active tab
        QWidget* currentWidget = tabWidget->currentWidget();
        if (!currentWidget) {
            qWarning() << "No widget found in tab index:" << currentIndex;
            continue;
        }

        CodeEditor* editor = qobject_cast<CodeEditor*>(currentWidget);
        if (!editor) {
            qWarning() << "Widget is not a CodeEditor at tab index:" << currentIndex;
            continue;
        }

        // Set the editor for the Find object and reset the cursor
        setEditor(editor);
        m_cursor = QTextCursor(m_editor->document());

        qDebug() << "Searching in tab index:" << currentIndex;
        if (search(backward)) {
            qDebug() << "Match found in tab index:" << currentIndex;
            return true; // Stop if a match is found
        }

        // Move to the next or previous tab
        currentIndex = backward ? (currentIndex - 1 + tabCount) % tabCount
                                : (currentIndex + 1) % tabCount;

    } while (currentIndex != startIndex);

    qDebug() << "No matches found across all tabs.";
    return false; // No matches found
}
