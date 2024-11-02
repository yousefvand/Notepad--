#include <QDebug>
#include <QApplication>
#include <QClipboard>
#include <QTextCursor>
#include <QStringList>
#include <QPlainTextEdit>
#include "../document.h"
#include "../codeeditor.h"
#include "textoperations.h"
#include "../helpers.h"
#include "../settings.h"

TextOperations::TextOperations(QTabWidget* documentsTab)
    : m_documentsTab(documentsTab) {
    if (!m_documentsTab) {
        qDebug() << "Error: documentsTab is null!";
    }
}

void TextOperations::undo() {
    Document* doc = Helpers::getCurrentDocument(m_documentsTab);
    if (doc) {
        doc->editor()->undo();
        qDebug() << "Undo performed.";
    } else {
        qDebug() << "No document available for undo.";
    }
}

void TextOperations::redo() {
    Document* doc = Helpers::getCurrentDocument(m_documentsTab);
    if (doc) {
        doc->editor()->redo();
        qDebug() << "Redo performed.";
    } else {
        qDebug() << "No document available for redo.";
    }
}

void TextOperations::cut() {
    Document* doc = Helpers::getCurrentDocument(m_documentsTab);
    if (!doc) {
        qDebug() << "No document available for cut.";
        return;
    }

    QPlainTextEdit* editor = doc->editor();
    if (editor->textCursor().hasSelection()) {
        qDebug() << "Cutting selected text.";

        // Copy the selected text to the clipboard
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setText(editor->textCursor().selectedText());

        // Remove the selected text from the editor
        editor->cut();
    } else {
        qDebug() << "No text selected to cut.";
    }
}

void TextOperations::copy() {
    Document* doc = Helpers::getCurrentDocument(m_documentsTab);
    if (!doc) {
        qDebug() << "No document available for copy.";
        return;
    }

    QPlainTextEdit* editor = doc->editor();
    if (editor->textCursor().hasSelection()) {
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setText(editor->textCursor().selectedText());  // Copy selected text
        qDebug() << "Copy performed.";
    } else {
        qDebug() << "No text selected to copy.";
    }
}

void TextOperations::paste() {
    Document* doc = Helpers::getCurrentDocument(m_documentsTab);
    if (!doc) {
        qDebug() << "No document available for paste.";
        return;
    }

    QPlainTextEdit* editor = doc->editor();
    QClipboard* clipboard = QApplication::clipboard();
    QString clipboardText = clipboard->text();

    if (!clipboardText.isEmpty()) {
        // Insert the clipboard text at the current cursor position
        editor->insertPlainText(clipboardText);
        qDebug() << "Pasted text:" << clipboardText;
    } else {
        qDebug() << "Clipboard is empty, nothing to paste.";
    }
}

void TextOperations::deleteText() {
    Document* doc = Helpers::getCurrentDocument(m_documentsTab);
    if (!doc) {
        qDebug() << "No document available for delete operation.";
        return;
    }

    QPlainTextEdit* editor = doc->editor();
    QTextCursor cursor = editor->textCursor();

    if (cursor.hasSelection()) {
        cursor.removeSelectedText();  // Delete selected text
        qDebug() << "Deleted selected text.";
    } else {
        cursor.deleteChar();  // Delete the character in front of the cursor
        qDebug() << "Deleted character ahead of cursor.";
    }

    // Ensure the cursor remains updated in the editor
    editor->setTextCursor(cursor);
}

void TextOperations::selectAll() {
    Document* doc = Helpers::getCurrentDocument(m_documentsTab);
    if (!doc) {
        qDebug() << "No document available for select all operation.";
        return;
    }

    QPlainTextEdit* editor = doc->editor();
    editor->selectAll();  // Select all the text in the editor
    qDebug() << "Selected all text.";
}

void TextOperations::handleCheckboxToggle(bool checked) {
    Document* doc = Helpers::getCurrentDocument(m_documentsTab);
    if (!doc) return;

    CodeEditor* editor = doc->editor();
    QTextCursor cursor = editor->textCursor();

    if (checked) {
        // Keep the current selection when the checkbox is checked.
        return;
    }

    // Checkbox is unchecked: Select the unselected part of the line.
    if (cursor.hasSelection()) {
        // Capture the current selection's start and end positions.
        int selectionStart = cursor.selectionStart();
        int selectionEnd = cursor.selectionEnd();

        // Clear the current selection.
        cursor.clearSelection();

        // Determine the start and end positions of the current line.
        int lineStart = cursor.block().position();
        int lineEnd = lineStart + cursor.block().length() - 1;

        // Select the unselected part of the line.
        if (selectionStart > lineStart) {
            // Select from the start of the line to the start of the previous selection.
            cursor.setPosition(lineStart);
            cursor.setPosition(selectionStart, QTextCursor::KeepAnchor);
        } else if (selectionEnd < lineEnd) {
            // Select from the end of the previous selection to the end of the line.
            cursor.setPosition(selectionEnd);
            cursor.setPosition(lineEnd, QTextCursor::KeepAnchor);
        }

        // Apply the new selection to the editor.
        editor->setTextCursor(cursor);
    }
}

void TextOperations::selectRestOfLine(QPlainTextEdit* editor) {
    QTextCursor cursor = editor->textCursor();
    cursor.clearSelection();  // Deselect any existing selection
    cursor.movePosition(QTextCursor::StartOfLine);
    cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    editor->setTextCursor(cursor);
    qDebug() << "Selected rest of the line.";
}

void TextOperations::convertToUpperCase() {
    Document* currentDoc = Helpers::getCurrentDocument(m_documentsTab);
    if (!currentDoc) {
        qDebug() << "No active document.";
        return;
    }

    QTextCursor cursor = currentDoc->editor()->textCursor();

    if (!cursor.hasSelection()) {
        qDebug() << "No text selected.";
        return;
    }

    // Get the selected text and convert it to uppercase
    QString selectedText = cursor.selectedText();
    QString upperText = selectedText.toUpper();

    // Replace the selected text with its uppercase version
    cursor.insertText(upperText);

    qDebug() << "Selected text converted to uppercase.";
}

void TextOperations::convertToLowerCase() {
    Document* currentDoc = Helpers::getCurrentDocument(m_documentsTab);
    if (!currentDoc) {
        qDebug() << "No active document.";
        return;
    }

    QTextCursor cursor = currentDoc->editor()->textCursor();

    if (!cursor.hasSelection()) {
        qDebug() << "No text selected.";
        return;
    }

    // Get the selected text and convert it to lowercase
    QString selectedText = cursor.selectedText();
    QString lowerText = selectedText.toLower();

    // Replace the selected text with its lowercase version
    cursor.insertText(lowerText);

    qDebug() << "Selected text converted to lowercase.";
}

void TextOperations::duplicateCurrentLine() {
    Document* doc = Helpers::getCurrentDocument(m_documentsTab);
    if (!doc) return;

    QPlainTextEdit* editor = doc->editor();
    QTextCursor cursor = editor->textCursor();

    cursor.select(QTextCursor::LineUnderCursor);
    QString selectedLine = cursor.selectedText();

    cursor.movePosition(QTextCursor::EndOfLine);

    if (Settings::instance()->eol() == Settings::OldMac) {
        cursor.insertText("\r" + selectedLine);
    } else if (Settings::instance()->eol() == Settings::Windows) {
        cursor.insertText("\r\n" + selectedLine);
    } else { // Unix
        cursor.insertText("\n" + selectedLine);
    }

    editor->setTextCursor(cursor);
}

void TextOperations::deleteCurrentLine() {
    Document* doc = Helpers::getCurrentDocument(m_documentsTab);
    if (!doc) return;

    QPlainTextEdit* editor = doc->editor();
    QTextCursor cursor = editor->textCursor();

    cursor.select(QTextCursor::LineUnderCursor);
    cursor.removeSelectedText();

    if (!cursor.atEnd()) {
        cursor.deleteChar();
    }

    editor->setTextCursor(cursor);
}

void TextOperations::moveLineUp() {
    Document* doc = Helpers::getCurrentDocument(m_documentsTab);
    if (!doc) return;

    QPlainTextEdit* editor = doc->editor();
    QTextCursor cursor = editor->textCursor();
    int currentPos = cursor.position();

    // Select the current line
    cursor.select(QTextCursor::LineUnderCursor);
    QString currentLine = cursor.selectedText();

    // Move cursor to the previous line
    cursor.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor);
    cursor.select(QTextCursor::LineUnderCursor);
    QString previousLine = cursor.selectedText();

    // If we're at the top, do nothing
    if (previousLine.isEmpty()) return;

    // Swap the lines
    cursor.insertText(currentLine + "\n");
    cursor.movePosition(QTextCursor::StartOfLine);
    cursor.insertText(previousLine);

    // Restore the cursor position to the moved line
    cursor.setPosition(currentPos - previousLine.length() - 1);
    editor->setTextCursor(cursor);
}

void TextOperations::moveLineDown() {
    Document* doc = Helpers::getCurrentDocument(m_documentsTab);
    if (!doc) return;

    QPlainTextEdit* editor = doc->editor();
    QTextCursor cursor = editor->textCursor();

    // Select the current line
    cursor.select(QTextCursor::LineUnderCursor);
    QString currentLine = cursor.selectedText();

    // Move cursor to the next line
    if (!cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor)) {
        return;  // If we're already on the last line, do nothing
    }

    // Select the next line
    cursor.select(QTextCursor::LineUnderCursor);
    QString nextLine = cursor.selectedText();

    // Perform the swap: Replace next line with the current line
    cursor.insertText(currentLine);

    // Move back to the original line's position
    cursor.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor);

    // Insert the next line where the current line was
    cursor.select(QTextCursor::LineUnderCursor);
    cursor.insertText(nextLine);

    // Move the cursor to the newly moved line
    cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor);
    editor->setTextCursor(cursor);
}

