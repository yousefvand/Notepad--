#include "document.h"
#include "codeeditor.h"
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QtConcurrent>
#include <QScrollBar>
#include <QDebug>
#include <QApplication>
#include <QtConcurrent>
#include <QFutureWatcher>
#include <QtConcurrent>
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

    // Call loadContent instead of loadEntireFile
    loadContent();
}

void Document::startFileLoadingAfterUIReady() {
    if (!m_filePath.isEmpty()) {
        qDebug() << "Starting file loading after UI is ready";
        openFile(m_filePath);  // Start loading the file
    } else {
        qDebug() << "No file path set. Cannot load file.";
    }
}

void Document::loadContent() {
    if (!m_file.isOpen()) {
        qDebug() << "File not open. Cannot load content.";
        return;
    }

    // Reset progress state at the start of the new file load
    emit loadingProgress(0);  // Start progress at 0%
    emit loadingStarted();     // Emit that loading has started
    qDebug() << "Loading started signal emitted";

    QByteArray content;
    qint64 fileSize = m_file.size();
    qint64 bytesRead = 0;

    const qint64 chunkSize = (fileSize > 1024 * 1024) ? fileSize / 100 : fileSize;  // Dynamic chunk size

    qDebug() << "Loading entire file content. File size: " << fileSize << " Chunk size: " << chunkSize;

    // Reset the editor before new content load
    editor->clear();

    // Move file loading to a separate thread to avoid blocking the UI
    QtConcurrent::run([this, fileSize, chunkSize]() {
        QByteArray content;
        qint64 bytesRead = 0;

        // Reading file in chunks
        while (bytesRead < fileSize) {
            QByteArray chunk = m_file.read(chunkSize);
            if (chunk.isEmpty()) {
                qDebug() << "Error reading file.";
                emit loadingProgress(100);  // Set progress to 100% on error
                break;
            }

            content.append(chunk);
            bytesRead += chunk.size();

            // Use a queued connection to ensure the UI is updated
            QMetaObject::invokeMethod(QApplication::instance(), []() {
                QApplication::processEvents();
            }, Qt::QueuedConnection);
        }

        m_file.close();
        m_originalText = QString::fromUtf8(content);

        // Insert text into the editor
        QMetaObject::invokeMethod(this, [this, fileSize, content]() {
            QTextCursor cursor(editor->document());
            qint64 totalSize = m_originalText.size();
            qint64 insertSize = 0;

            while (insertSize < totalSize) {
                QString insertChunk = m_originalText.mid(insertSize, fileSize / 100);
                cursor.movePosition(QTextCursor::End);
                cursor.insertText(insertChunk);
                insertSize += insertChunk.size();

                // Calculate progress based on the actual text inserted
                int progress = static_cast<int>((static_cast<double>(insertSize) / totalSize) * 100);
                emit loadingProgress(progress);
                qDebug() << "Loading progress (text insertion): " << progress;

                // Process UI events to ensure progress bar updates smoothly
                QApplication::processEvents();
            }

            emit loadingProgress(100);  // Set progress to 100% after insertion
            emit loadingFinished();  // Emit that loading has finished
            qDebug() << "Loading finished signal emitted";

            // Move cursor to the beginning of the document
            cursor.movePosition(QTextCursor::Start);
            editor->setTextCursor(cursor);
        }, Qt::QueuedConnection);
    });
}

void Document::loadContentAsync() {
    if (!m_file.isOpen()) {
        qDebug() << "File not open. Cannot load content.";
        return;
    }

    qDebug() << "Loading entire file content. File size: " << m_file.size();
    emit loadingStarted();  // Emit signal that loading has started

    QByteArray content;
    qint64 fileSize = m_file.size();
    qint64 bytesRead = 0;
    const qint64 chunkSize = 1024 * 1024;  // Simulate progress in chunks of 1MB
    while (bytesRead < fileSize) {
        QByteArray chunk = m_file.read(chunkSize);
        content.append(chunk);
        bytesRead += chunk.size();
        int progress = static_cast<int>((static_cast<double>(bytesRead) / fileSize) * 100);
        emit loadingProgress(progress);  // Emit progress update signal
    }

    m_file.close();

    m_originalText = QString::fromUtf8(content);

    // Insert content into the editor
    QMetaObject::invokeMethod(this, [this, content]() {
        QTextCursor cursor(editor->document());
        cursor.movePosition(QTextCursor::End);
        cursor.insertText(m_originalText);

        if (syntaxHighlighter) {
            syntaxHighlighter->rehighlight();
        }

        cursor.movePosition(QTextCursor::Start);
        editor->setTextCursor(cursor);
        emit loadingFinished();  // Emit signal that loading is complete
    }, Qt::QueuedConnection);
}

void Document::saveFile() {
    QFuture<void> future = QtConcurrent::run([this]() {
        QString currentText = editor->toPlainText();
        if (currentText == m_originalText) {
            qDebug() << "No changes to save.";
            return;  // No changes to save
        }

        emit savingStarted();  // Emit that saving has started
        qDebug() << "Saving started signal emitted";

        QFile file(m_filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Cannot open file for writing: " << file.errorString();
            return;
        }

        qint64 fileSize = currentText.toUtf8().size();
        qint64 bytesWritten = 0;
        const qint64 chunkSize = 1024 * 1024;  // Simulate progress in chunks of 1MB

        while (bytesWritten < fileSize) {
            QByteArray chunk = currentText.toUtf8().mid(bytesWritten, chunkSize);
            file.write(chunk);
            bytesWritten += chunk.size();
            int progress = static_cast<int>((static_cast<double>(bytesWritten) / fileSize) * 100);

            emit savingProgress(progress);  // Emit progress
            qDebug() << "Saving progress: " << progress;
        }

        file.close();
        m_originalText = currentText;
        emit savingFinished();  // Emit that saving has finished
        qDebug() << "Saving finished signal emitted";
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
