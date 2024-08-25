#include "document.h"
#include "codeeditor.h"
#include "languages/languagemanager.h"
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QtConcurrent>
#include <QScrollBar>
#include <QCryptographicHash>
#include <QDebug>

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

    // Load the entire file content for simplicity
    QByteArray content = m_file.readAll();
    editor->setPlainText(QString::fromUtf8(content));
    editor->moveCursor(QTextCursor::Start);

    // Calculate the original hash of the file asynchronously
    QtConcurrent::run([this]() {
        m_originalHash = calculateMD5Stream(&m_file);
        qDebug() << "Original MD5 calculated:" << m_originalHash.toHex();
    });

    m_file.close();
}

void Document::trackChanges() {
    // Basic change tracking (full document, for simplicity)
    m_currentText = editor->toPlainText();
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

void Document::loadContent() {
    if (!m_file.isOpen()) {
        qDebug() << "File is not open, cannot load content.";
        return;
    }

    // Calculate the size of the portion to read
    qint64 bytesToRead = m_endPointer - m_startPointer;

    // Ensure the reading is within bounds
    if (m_startPointer < 0 || m_startPointer >= m_fileSize || bytesToRead <= 0) {
        qDebug() << "Invalid pointers or no data to read.";
        return;
    }

    m_file.seek(m_startPointer);  // Seek to the start pointer

    QByteArray content = m_file.read(bytesToRead);
    if (content.isEmpty()) {
        qDebug() << "No content read from the file.";
        return;
    }

    editor->setPlainText(QString::fromUtf8(content));  // Set content to the editor
    editor->moveCursor(QTextCursor::Start);  // Move cursor to the start of the document

    qDebug() << "File content loaded successfully. Start pointer:" << m_startPointer << ", End pointer:" << m_endPointer;
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

    QTextStream out(&file);
    out << editor->toPlainText();

    file.close();

    // Recalculate and store the MD5 hash of the saved file
    m_originalHash = calculateMD5Stream(&file);

    qDebug() << "File saved. New MD5:" << m_originalHash.toHex();
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
    if (!file->isOpen()) {
        if (!file->open(QIODevice::ReadOnly)) {
            qDebug() << "Failed to open file for MD5 calculation:" << file->errorString();
            return QByteArray();
        }
    }

    file->seek(0);  // Ensure we start from the beginning of the file

    // Read the entire file content and update the hash
    while (!file->atEnd()) {
        QByteArray buffer = file->read(1024 * 1024);  // Read in chunks (1MB here)
        hash.addData(buffer);
    }

    file->close();  // Close the file if it was opened here
    return hash.result();  // Return the calculated MD5 hash
}

QByteArray Document::calculateModifiedMD5() {
    QFile file(m_filePath);
    return calculateMD5Stream(&file);
}

bool Document::closeDocument() {
    QByteArray currentHash = calculateModifiedMD5();
    qDebug() << "Original MD5:" << m_originalHash.toHex();
    qDebug() << "Current MD5:" << currentHash.toHex();

    if (currentHash != m_originalHash) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::warning(this, tr("Unsaved Changes"),
                                     tr("You have unsaved changes."),
                                     QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (reply == QMessageBox::Save) {
            saveFile();
            return true;
        } else if (reply == QMessageBox::Discard) {
            return true;
        } else {
            return false;
        }
    }

    return true;
}

void Document::goToLineNumberInEditor() {
    bool ok;
    int lineNumber = QInputDialog::getInt(this, tr("Go to Line"),
                                          tr("Line number:"), 1, 1, INT_MAX, 1, &ok);

    if (ok) {
        QTextCursor cursor = editor->textCursor();
        cursor.movePosition(QTextCursor::Start); // Move to the start of the editor

        for (int i = 1; i < lineNumber; ++i) {
            cursor.movePosition(QTextCursor::Down);

            if (cursor.atEnd()) {
                QMessageBox::warning(this, tr("Line Number Out of Bounds"),
                                     tr("The specified line number is outside the bounds of the file."));
                return;
            }
        }

        editor->setTextCursor(cursor);
        editor->ensureCursorVisible();
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
