#include "document.h"
#include "codeeditor.h"
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QtConcurrent>
#include <QScrollBar>
#include <QDebug>

Document::Document(const QString &filePath, QWidget *parent)
    : QWidget(parent), m_filePath(filePath), m_fileSize(0), m_startPointer(0), m_endPointer(0), syntaxHighlighter(nullptr) {

    qDebug() << "Initializing Document for file:" << filePath;

    // Initialize the code editor and layout
    editor = new CodeEditor(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(editor);
    setLayout(layout);

    qDebug() << "CodeEditor and layout initialized.";

    // Connect signals to corresponding slots
    connect(editor->verticalScrollBar(), &QScrollBar::valueChanged, this, &Document::updatePointers);
    connect(editor, &QPlainTextEdit::cursorPositionChanged, this, &Document::updatePointers);
    connect(editor, &QPlainTextEdit::textChanged, this, &Document::trackChanges);

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

#include <QtConcurrent>

void Document::openFile(const QString &filePath) {
    m_file.setFileName(filePath);
    if (!m_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), tr("Cannot open file: ") + m_file.errorString());
        return;
    }

    setFilePath(filePath);

    m_fileSize = m_file.size();
    m_startPointer = 0;
    m_endPointer = qMin(m_fileSize, static_cast<qint64>(CHUNK_SIZE)); // Load only the first 1MB

    // Load content in a separate thread
    QFuture<void> future = QtConcurrent::run(this, &Document::loadContentAsync);
}

void Document::loadContentAsync() {
    if (!m_file.isOpen()) {
        qDebug() << "File not open. Cannot load content.";
        return;
    }

    qDebug() << "Loading content in background from:" << m_startPointer << "to:" << m_endPointer;
    m_file.seek(m_startPointer);
    QByteArray content = m_file.read(m_endPointer - m_startPointer);

    // Use a Qt signal to update the editor content on the main thread
    QMetaObject::invokeMethod(this, [this, content]() {
        QTextCursor cursor(editor->document());
        cursor.movePosition(QTextCursor::End);
        cursor.insertText(QString::fromUtf8(content));

        qDebug() << "Content loaded and inserted into editor. Document size:" << editor->document()->characterCount();

        // Trigger syntax highlighting if available
        if (syntaxHighlighter) {
            syntaxHighlighter->rehighlight();
        }

        // Move cursor to trigger line highlighting
        cursor.movePosition(QTextCursor::Start);
        editor->setTextCursor(cursor);

    }, Qt::QueuedConnection);
}

void Document::loadContent() {
    if (!m_file.isOpen()) {
        qDebug() << "File not open. Cannot load content.";
        return;
    }

    qDebug() << "Loading content from:" << m_startPointer << "to:" << m_endPointer;
    m_file.seek(m_startPointer);
    QByteArray content = m_file.read(m_endPointer - m_startPointer);

    // Store the original content in the map
    QString originalText = QString::fromUtf8(content);
    m_originalSegments[m_startPointer] = originalText;

    // Insert the text into the editor
    QTextCursor cursor(editor->document());
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(originalText);

    qDebug() << "Original segment stored from " << m_startPointer << " to " << m_endPointer;
    qDebug() << "Loaded content. Current document size:" << editor->document()->characterCount();
}

void Document::updatePointers() {
    qDebug() << "updatePointers called. Cursor position:" << editor->textCursor().position();
    qint64 cursorPos = editor->textCursor().position();

    if (cursorPos < 0 || cursorPos >= m_fileSize) {
        qDebug() << "Cursor position out of range.";
        return;
    }

    // Load the next chunk if the cursor is near the end of the current chunk
    if (cursorPos >= m_endPointer - CHUNK_THRESHOLD && m_endPointer < m_fileSize) {
        qDebug() << "Loading next chunk. Current Start:" << m_startPointer << "End:" << m_endPointer;
        m_startPointer = m_endPointer;
        m_endPointer = qMin(m_startPointer + CHUNK_SIZE, m_fileSize);
        loadContent();
    }
}

void Document::trackChanges() {
    QString currentText = editor->toPlainText();
    QString currentSegmentText = currentText.mid(m_startPointer, m_endPointer - m_startPointer);

    qDebug() << "Tracking changes...";
    qDebug() << "Current segment start: " << m_startPointer;
    qDebug() << "Original segment: " << m_originalSegments[m_startPointer];
    qDebug() << "Current segment: " << currentSegmentText;

    if (m_originalSegments.contains(m_startPointer)) {
        if (currentSegmentText != m_originalSegments[m_startPointer]) {
            m_changedSegments[m_startPointer] = currentSegmentText;
            qDebug() << "Change detected and stored for segment starting at:" << m_startPointer;
        } else {
            m_changedSegments.remove(m_startPointer);
            qDebug() << "Segment matches original; no change detected.";
        }
    } else {
        qDebug() << "No original segment found for comparison.";
    }
}

void Document::saveFile() {
    if (m_changedSegments.isEmpty()) {
        qDebug() << "No changes to save.";
        return; // No changes to save
    }

    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadWrite)) {
        QMessageBox::warning(this, tr("Error"), tr("Cannot open file for writing: ") + file.errorString());
        return;
    }

    // Iterate over the changed segments and write them to the file
    for (auto it = m_changedSegments.constBegin(); it != m_changedSegments.constEnd(); ++it) {
        qint64 segmentStart = it.key();
        QString changedText = it.value();

        file.seek(segmentStart);
        file.write(changedText.toUtf8());

        qDebug() << "Saved changes to segment starting at:" << segmentStart;
    }

    file.close();
    m_changedSegments.clear(); // Clear the map after saving
    qDebug() << "File saved successfully.";
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
