#include "document.h"
#include "codeeditor.h"
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QtConcurrent>
#include <QScrollBar>

Document::Document(const QString &filePath, QWidget *parent)
    : QWidget(parent), m_filePath(filePath), m_fileSize(0), m_startPointer(0), m_endPointer(0), syntaxHighlighter(nullptr) {

    editor = new CodeEditor(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(editor);
    setLayout(layout);

    connect(editor->verticalScrollBar(), &QScrollBar::valueChanged, this, &Document::updatePointers);
    connect(editor, &QPlainTextEdit::cursorPositionChanged, this, &Document::updatePointers);
    connect(editor, &QPlainTextEdit::textChanged, this, &Document::trackChanges);

    if (!filePath.isEmpty()) {
        openFile(filePath);
    }

    m_changedSegments.clear();
    m_currentText.clear();
}

void Document::setFilePath(const QString &path) {
    m_filePath = path;
    m_fileExtension = QFileInfo(path).suffix();
    qDebug() << "File opened with extension:" << m_fileExtension;
    QString language = LanguageManager::getLanguageFromExtension(m_fileExtension);
    qDebug() << "Detected language:" << language;
    applySyntaxHighlighter(language);
}

QString Document::filePath() const {
    return m_filePath;
}

QString Document::getLanguage() const {
    return m_language;
}

void Document::applySyntaxHighlighter(const QString &language) {
    if (syntaxHighlighter) {
        delete syntaxHighlighter;
        syntaxHighlighter = nullptr;
    }

    m_language = language;  // Set the current language based on the argument

    // Use LanguageManager to create the appropriate highlighter based on the language directly
    syntaxHighlighter = LanguageManager::createHighlighterForExtension(language, editor->document());

    if (syntaxHighlighter) {
        syntaxHighlighter->rehighlight(); // Rehighlight the current text in the editor
        qDebug() << "Applied syntax highlighter for language:" << language;
    } else {
        qDebug() << "No syntax highlighter found for language:" << language;
    }
}

void Document::openFile(const QString &filePath) {
    m_file.setFileName(filePath);
    if (!m_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), tr("Cannot open file: ") + m_file.errorString());
        return;
    }

    setFilePath(filePath);

    m_fileSize = m_file.size();
    m_startPointer = 0;
    m_endPointer = qMin(m_fileSize, m_startPointer + 1024 * 1024); // Load first 1MB

    loadContent();

    // Calculate the original hash of the file asynchronously
    QtConcurrent::run([this]() {
        m_originalHash = calculateMD5Stream(&m_file);
    });
}

void Document::loadContent() {
    if (!m_file.isOpen()) return;

    m_file.seek(m_startPointer);
    QByteArray content = m_file.read(m_endPointer - m_startPointer);
    editor->setPlainText(QString::fromUtf8(content));
    editor->moveCursor(QTextCursor::Start);
}

void Document::updatePointers() {
    qint64 cursorPos = editor->textCursor().position();
    if (cursorPos < 0 || cursorPos >= editor->toPlainText().length()) {
        return; // Invalid cursor position
    }

    qint64 pageSize = m_endPointer - m_startPointer;

    if (cursorPos >= (pageSize - 100) && m_endPointer < m_fileSize) {
        m_startPointer = m_endPointer;
        m_endPointer = qMin(m_startPointer + pageSize, m_fileSize);
        loadContent();
    } else if (cursorPos < 100 && m_startPointer > 0) {
        m_endPointer = m_startPointer;
        m_startPointer = qMax(m_startPointer - pageSize, qint64(0));
        m_endPointer = m_startPointer + pageSize;
        loadContent();
    }
}

void Document::trackChanges() {
    m_currentText = editor->toPlainText();
    m_changedSegments[m_startPointer] = m_currentText;
}

void Document::saveFile() {
    if (m_filePath.isEmpty()) {
        saveFileAs(QFileDialog::getSaveFileName(this, tr("Save File As"), "", tr("Text Files (*.txt);;All Files (*)")));
        return;
    }

    QFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), tr("Cannot open file for writing: ") + file.errorString());
        return;
    }

    // Use QTextStream for writing
    QTextStream out(&file);
    out << editor->toPlainText(); // Write the entire content of the editor to the file

    // After saving, calculate the new hash asynchronously
    QtConcurrent::run([this]() {
        m_originalHash = calculateMD5Stream(&m_file);
    });

    file.close();
}

void Document::saveFileAs(const QString &newFilePath) {
    if (newFilePath.isEmpty()) {
        return; // User canceled the Save As dialog, do nothing
    }
    m_filePath = newFilePath;
    saveFile();
}

QByteArray Document::calculateMD5Stream(QFile *file) {
    QCryptographicHash hash(QCryptographicHash::Md5);
    file->seek(0);

    QByteArray buffer;
    while (!file->atEnd()) {
        buffer = file->read(1024 * 1024); // Read in chunks
        hash.addData(buffer);
    }

    return hash.result();
}

QByteArray Document::calculateModifiedMD5() {
    QString currentContent = editor->toPlainText();
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(currentContent.toUtf8());
    return hash.result();
}

bool Document::closeDocument() {
    // Special case: if the document is empty, allow closing without checking the hash
    if (editor->toPlainText().isEmpty() && m_filePath.isEmpty()) {
        return true;
    }

    QByteArray currentHash = calculateModifiedMD5();

    if (currentHash != m_originalHash) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::warning(this, tr("Unsaved Changes"),
                                     tr("You have unsaved changes."),
                                     QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (reply == QMessageBox::Save) {
            QString savePath = QFileDialog::getSaveFileName(this, tr("Save File As"), "", tr("Text Files (*.txt);;All Files (*)"));
            if (savePath.isEmpty()) {
                return false; // User canceled the Save As dialog, don't close the document
            }
            saveFileAs(savePath);
            return true;
        } else if (reply == QMessageBox::Discard) {
            return true; // Allow closing without saving
        } else {
            return false; // Cancel closing
        }
    }

    return true; // No unsaved changes, allow closing
}

void Document::goToLineNumberInEditor() {
    bool ok;
    int lineNumber = QInputDialog::getInt(this, tr("Go to Line"),
                                          tr("Line number:"), 1, 1, INT_MAX, 1, &ok);

    if (ok) {
        QTextCursor cursor = editor->textCursor();
        cursor.movePosition(QTextCursor::Start); // Move to the start of the editor

        // Move the cursor down to the desired line number
        for (int i = 1; i < lineNumber; ++i) {
            cursor.movePosition(QTextCursor::Down);

            // Check if the cursor has moved to the end of the document
            if (cursor.atEnd()) {
                QMessageBox::warning(this, tr("Line Number Out of Bounds"),
                                     tr("The specified line number is outside the bounds of the file."));
                return; // Exit if the line number is out of bounds
            }
        }

        editor->setTextCursor(cursor);
        editor->ensureCursorVisible(); // Ensure the cursor is visible after moving
    }
}

void Document::goToLineNumberInText(QWidget* parent) {
    bool ok;
    int lineNumber = QInputDialog::getInt(parent, tr("Go to Line"),
                                          tr("Line number:"), 1, 1, INT_MAX, 1, &ok);
    if (ok) {
        QTextCursor cursor = editor->textCursor();
        cursor.movePosition(QTextCursor::Start);

        int currentLineNumber = 0;
        while (currentLineNumber < lineNumber - 1 && !cursor.atEnd()) {
            cursor.movePosition(QTextCursor::NextBlock);
            ++currentLineNumber;
        }

        editor->setTextCursor(cursor);
    }
}
