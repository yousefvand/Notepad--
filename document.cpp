#include "document.h"
#include "codeeditor.h"
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QtConcurrent>
#include <QScrollBar>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>

Document::Document(const QString &filePath, QWidget *parent)
    : QWidget(parent), m_filePath(filePath), m_fileSize(0), syntaxHighlighter(nullptr) {

    qDebug() << "Initializing Document for file:" << filePath;

    // Initialize the code editor and layout
    editor = new CodeEditor(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(editor);
    setLayout(layout);

    qDebug() << "CodeEditor and layout initialized.";
    qDebug() << "Signal-slot connections set up.";

    // If a file path is provided, open the file
    if (!filePath.isEmpty()) {
        openFile(filePath);
        qDebug() << "File opened:" << filePath;
    } else {
        qDebug() << "No file path provided.";
    }

    // Initialize the text and changed segments map
    m_changedSegments.clear();
    m_currentText.clear();

    qDebug() << "Document initialized successfully.";
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

    m_language = language;

    syntaxHighlighter = LanguageManager::createHighlighterForExtension(language, editor->document());

    if (syntaxHighlighter) {
        syntaxHighlighter->rehighlight();
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

    // Load the entire content in a separate thread to avoid blocking the UI
    QFuture<void> future = QtConcurrent::run(this, &Document::loadEntireFile);
}

void Document::loadEntireFile() {
    if (!m_file.isOpen()) {
        qDebug() << "File not open. Cannot load content.";
        return;
    }

    qDebug() << "Loading entire file content into memory. File size:" << m_fileSize;

    // Read the entire file content
    QByteArray content = m_file.readAll();
    m_file.close(); // Close the file as it's now fully loaded into memory

    // Store the original content
    m_originalText = QString::fromUtf8(content);

    // Update the editor content on the UI thread
    QMetaObject::invokeMethod(this, [this]() {
        QTextCursor cursor(editor->document());
        cursor.movePosition(QTextCursor::Start);
        cursor.insertText(m_originalText);

        qDebug() << "File content loaded and inserted into editor. Document size:" << editor->document()->characterCount();

        // Trigger syntax highlighting if available
        if (syntaxHighlighter) {
            syntaxHighlighter->rehighlight();
        }

        // Move cursor to trigger line highlighting
        cursor.movePosition(QTextCursor::Start);
        editor->setTextCursor(cursor);
    }, Qt::QueuedConnection);
}

void Document::loadContentAsync() {
    if (!m_file.isOpen()) {
        qDebug() << "File not open. Cannot load content.";
        return;
    }

    qDebug() << "Loading entire content in background. File size: " << m_file.size();

    // Read the entire file content at once
    QByteArray content = m_file.readAll();
    m_file.close(); // Close the file after reading

    // Use a Qt signal to update the editor content on the main thread
    QMetaObject::invokeMethod(this, [this, content]() {
        QTextCursor cursor(editor->document());
        cursor.movePosition(QTextCursor::End);
        cursor.insertText(QString::fromUtf8(content));

        qDebug() << "Entire content loaded and inserted into editor. Document size: " << editor->document()->characterCount();

        // Trigger syntax highlighting if available
        if (syntaxHighlighter) {
            syntaxHighlighter->rehighlight();
        }

        // Move cursor to the beginning to trigger line highlighting
        cursor.movePosition(QTextCursor::Start);
        editor->setTextCursor(cursor);
    }, Qt::QueuedConnection);
}

void Document::loadContent() {
    if (!m_file.isOpen()) {
        qDebug() << "File not open. Cannot load content.";
        return;
    }

    qDebug() << "Loading entire file content. File size: " << m_file.size();

    // Read the entire file content at once
    QByteArray content = m_file.readAll();
    m_file.close(); // Close the file after reading

    // Store the original content as a single string
    m_originalText = QString::fromUtf8(content);

    // Insert the text into the editor
    QTextCursor cursor(editor->document());
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(m_originalText);

    qDebug() << "Entire content loaded and inserted into editor. Document size: " << editor->document()->characterCount();
}

    void Document::saveFile() {
    // Run the save operation in a background thread
    QFuture<void> future = QtConcurrent::run([this]() {
        QString currentText = editor->toPlainText();
        if (currentText == m_originalText) {
            qDebug() << "No changes to save.";
            return; // No changes to save
        }

        QFile file(m_filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Cannot open file for writing: " << file.errorString();
            return;
        }

        // Write the entire content back to the file
        file.write(currentText.toUtf8());
        file.close();

        // Update the original text after saving
        m_originalText = currentText;

        qDebug() << "File saved successfully.";
    });
}

void Document::saveFileAs(const QString &newFilePath) {
    if (newFilePath.isEmpty()) {
        return;
    }
    m_filePath = newFilePath;
    saveFile();
}

bool Document::compareText(const QString &text1, const QString &text2) {
    if (text1.length() != text2.length()) {
        return false; // Different lengths means they are not the same
    }

    for (int i = 0; i < text1.length(); ++i) {
        if (text1[i] != text2[i]) {
            return false; // Early exit on first mismatch
        }
    }

    return true; // Texts are identical
}

bool Document::closeDocument() {
    if (checkForUnsavedChanges()) {
        return promptForSave();
    }

    qDebug() << "No unsaved changes detected. Safe to close.";
    return true; // No changes detected, safe to close
}

bool Document::promptForSave() {
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

void Document::goToLineNumberInEditor() {
    bool ok;
    int lineNumber = QInputDialog::getInt(this, tr("Go to Line"),
                                          tr("Line number:"), 1, 1, INT_MAX, 1, &ok);

    if (ok) {
        QTextCursor cursor = editor->textCursor();
        cursor.movePosition(QTextCursor::Start);

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

bool Document::checkForUnsavedChanges() {
    if (m_changedSegments.isEmpty()) {
        qDebug() << "No changes detected, no need to show 'unsaved changes' dialog.";
        return false; // No changes, safe to close
    }

    QFile originalFile(m_filePath);
    if (!originalFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), tr("Cannot open the file: ") + m_filePath);
        qDebug() << "Cannot open file for comparison.";
        return true; // Assume there are changes if we can't open the file
    }

    QTextStream originalStream(&originalFile);

    // Iterate over the changed segments
    for (auto it = m_changedSegments.constBegin(); it != m_changedSegments.constEnd(); ++it) {
        qint64 segmentStart = it.key();
        QString changedText = it.value();

        // Read the corresponding section from the original file
        originalFile.seek(segmentStart);
        QString originalText = originalStream.read(changedText.length());

        // Compare the changed text with the original text using compareText
        if (!compareText(changedText, originalText)) {
            qDebug() << "Detected changes that require saving.";
            return true; // Unsaved changes detected
        }
    }

    qDebug() << "No unsaved changes detected after comparison.";
    return false; // No changes detected
}

void Document::mirrorChangesToMemory(qint64 segmentStart, const QString &text) {
    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadWrite)) {
        qDebug() << "Cannot open file for writing:" << file.errorString();
        return;
    }

    file.seek(segmentStart);
    file.write(text.toUtf8());
    file.close();

    qDebug() << "Changes mirrored to memory at position:" << segmentStart;
}
