#include "document.h"
#include "codeeditor.h"
#include "mainwindow.h"
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QtConcurrent>
#include <QScrollBar>
#include <QProgressBar>
#include <QStatusBar>
#include <QMainWindow>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <QTimer>
#include <QCoreApplication>
#include <QFileInfo>

Document::Document(const QString &filePath, QWidget *parent)
    : QWidget(parent), m_filePath(filePath), m_fileSize(0), syntaxHighlighter(nullptr), m_mmap(nullptr) {

    qDebug() << "Initializing Document for file:" << filePath;

    // Initialize the code editor and layout
    editor = new CodeEditor(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(editor);
    setLayout(layout);

    qDebug() << "CodeEditor and layout initialized.";

    // Connect signals to corresponding slots
    connect(editor, &QPlainTextEdit::textChanged, this, &Document::trackChanges);

    qDebug() << "Signal-slot connections set up.";

    // If a file path is provided, open the file after ensuring GUI is fully loaded
    if (!filePath.isEmpty()) {
        QTimer::singleShot(0, this, &Document::openFileAfterGuiIsReady);
        qDebug() << "File opening scheduled for:" << filePath;
    } else {
        qDebug() << "No file path provided.";
    }

    // Initialize the text and changed segments map
    m_changedSegments.clear();
    m_currentText.clear();

    qDebug() << "Document initialized successfully.";
}

Document::~Document() {
    if (m_mmap && m_mmap != MAP_FAILED) {
        munmap(m_mmap, m_fileSize);
    }
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

void Document::openFileAfterGuiIsReady() {
    // Ensure that the GUI is fully loaded before starting file operations
    QTimer::singleShot(100, this, [this]() {
        openFile(m_filePath);
    });
}

void Document::openFile(const QString &filePath)
{
    emit updateProgress(0); // Reset progress at the start

    m_filePath = filePath;
    QFile file(m_filePath);

    if (!file.exists()) {
        qDebug() << "File does not exist:" << m_filePath;
        return;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file:" << m_filePath;
        return;
    }

    m_fileSize = file.size();
    qDebug() << "File size determined:" << m_fileSize;

    if (m_fileSize == 0) {
        qDebug() << "File is empty.";
        editor->clear();  // Clear editor for empty file
        emit updateProgress(100); // Show that the loading is complete
        moveCursorToFirstLine();  // Move cursor to the first line
        return;
    }

    // Use a small buffer size for safe processing
    const qint64 bufferSize = 64 * 1024; // 64 KB buffer
    qint64 loadedSize = 0;
    QByteArray buffer;
    buffer.reserve(bufferSize);

    editor->clear();  // Clear editor before loading new content

    while (loadedSize < m_fileSize) {
        qint64 bytesToRead = qMin(bufferSize, m_fileSize - loadedSize);
        buffer = file.read(bytesToRead);

        if (buffer.isEmpty()) {
            qDebug() << "Error reading file at position:" << loadedSize;
            break; // Error or end of file
        }

        editor->appendPlainText(QString::fromUtf8(buffer));
        loadedSize += bytesToRead;

        // Update progress
        int progress = static_cast<int>((static_cast<double>(loadedSize) / m_fileSize) * 100);
        emit updateProgress(progress);

        QCoreApplication::processEvents(); // Allow UI updates
    }

    file.close();

    emit updateProgress(100);  // Ensure the progress bar reaches 100% after loading the file
    moveCursorToFirstLine();  // Move cursor to the first line

    qDebug() << "Loaded content into editor. Document size:" << editor->toPlainText().size();
    qDebug() << "File opened successfully.";
}

void Document::moveCursorToFirstLine()
{
    QTextCursor cursor = editor->textCursor(); // Use the 'editor' variable
    cursor.movePosition(QTextCursor::Start);
    editor->setTextCursor(cursor);
}

void Document::loadContent() {
    if (m_mmap) {
        editor->setPlainText(QString::fromUtf8(m_mmap, m_fileSize));
    } else {
        qDebug() << "No memory-mapped file available to load.";
    }
}

void Document::trackChanges() {
    static QTimer *timer = nullptr;
    if (!timer) {
        timer = new QTimer(this);
        timer->setSingleShot(true);
        connect(timer, &QTimer::timeout, this, &Document::applyChangesToMemory);
    }
    timer->start(100); // Debounce delay (in milliseconds)
}

void Document::saveFile() {
    if (m_filePath.isEmpty()) {
        qDebug() << "No file path specified. Cannot save.";
        return;
    }

    emit updateProgress(0);  // Start progress bar at 0%

    QFile file(m_filePath);

    // Ensure the memory region is unmapped before proceeding
    if (m_mmap) {
        if (!file.unmap(reinterpret_cast<uchar*>(m_mmap))) {
            qDebug() << "Error unmapping memory region.";
            emit updateProgress(100);  // Set progress to 100% even if there's an error
            return;
        }
        m_mmap = nullptr;  // Reset the memory-mapped pointer after unmapping
    }

    // Try reopening the file for writing
    if (!file.open(QIODevice::ReadWrite)) {
        qDebug() << "Error reopening file for writing. Error:" << file.errorString();
        emit updateProgress(100);  // Set progress to 100% even if there's an error
        return;
    }

    const QByteArray content = editor->toPlainText().toUtf8();
    qint64 newSize = content.size();

    if (newSize != file.size()) {
        // Adjust the file size to match the new content size
        if (file.resize(newSize)) {
            m_fileSize = newSize;
        } else {
            qDebug() << "Error resizing file.";
            emit updateProgress(100);  // Set progress to 100% even if there's an error
            return;
        }
    }

    // Try to remap the file into memory
    m_mmap = reinterpret_cast<char*>(file.map(0, newSize));
    if (!m_mmap) {
        qDebug() << "Error remapping file. Error:" << file.errorString();
        file.close();
        emit updateProgress(100);  // Set progress to 100% even if there's an error
        return;
    }

    qint64 chunkSize = 64 * 1024;  // 64 KB chunks
    qint64 totalSize = newSize;
    for (qint64 i = 0; i < totalSize; i += chunkSize) {
        qint64 bytesToWrite = qMin(chunkSize, totalSize - i);
        memcpy(m_mmap + i, content.constData() + i, bytesToWrite);

        // Update the progress
        int progress = static_cast<int>((static_cast<double>(i + bytesToWrite) / totalSize) * 100);
        emit updateProgress(progress);

        QCoreApplication::processEvents(); // Allow UI updates
    }

    if (msync(m_mmap, newSize, MS_SYNC) != 0) {
        qDebug() << "Error syncing file to disk. Error:" << strerror(errno);
    }

    // Unmap memory and close the file
    if (!file.unmap(reinterpret_cast<uchar*>(m_mmap))) {
        qDebug() << "Error unmapping memory after saving.";
    }
    m_mmap = nullptr;
    file.close();

    emit updateProgress(100); // Update progress to 100% after saving
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
        return false;
    }

    for (int i = 0; i < text1.length(); ++i) {
        if (text1[i] != text2[i]) {
            return false;
        }
    }

    return true;
}

bool Document::closeDocument() {
    if (checkForUnsavedChanges()) {
        return promptForSave();
    }

    qDebug() << "No unsaved changes detected. Safe to close.";
    return true;
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

void Document::applyChangesToMemory() {
    QString text = editor->toPlainText();
    QByteArray textData = text.toUtf8();
    qint64 newSize = textData.size();

    // Fallback mechanism to ensure m_fileSize is correct
    if (m_fileSize == 0) {
        qDebug() << "Warning: m_fileSize is 0. Attempting to recalculate.";
        int fd = ::open(m_filePath.toStdString().c_str(), O_RDWR);
        if (fd != -1) {
            m_fileSize = lseek(fd, 0, SEEK_END);
            if (m_fileSize == -1) {
                qDebug() << "Failed to recalculate file size. Error:" << strerror(errno);
                ::close(fd);
                return;
            }
            qDebug() << "Recalculated file size:" << m_fileSize;
            ::close(fd);
        }
    }

    qDebug() << "Original file size:" << m_fileSize;
    qDebug() << "New content size:" << newSize;

    if (m_fileSize == 0) {
        qDebug() << "Error: File size is 0, cannot apply changes.";
        return;
    }

    if (newSize != m_fileSize) {
        // Unmap the old memory region
        if (m_mmap && m_mmap != MAP_FAILED) {
            qDebug() << "Unmapping old memory region.";
            munmap(m_mmap, m_fileSize);
        } else {
            qDebug() << "Memory region was not mapped or unmapping failed.";
        }

        int fd = ::open(m_filePath.toStdString().c_str(), O_RDWR);
        if (fd == -1) {
            qDebug() << "Error reopening file for writing. Error:" << strerror(errno);
            return;
        }

        qDebug() << "File opened successfully for resizing.";

        // Adjust the file size to match the new content size
        if (ftruncate(fd, newSize) == -1) {
            ::close(fd);
            qDebug() << "Error resizing file. Error:" << strerror(errno);
            return;
        }

        // Update the file size
        m_fileSize = newSize;

        // Remap the file to memory
        m_mmap = static_cast<char*>(mmap(nullptr, m_fileSize, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0));
        if (m_mmap == MAP_FAILED) {
            ::close(fd);
            qDebug() << "Error remapping file. Error:" << strerror(errno);
            return;
        }

        ::close(fd);
        qDebug() << "File remapped to memory successfully.";
    }

    // Copy the new text data to the memory-mapped region
    memcpy(m_mmap, textData.data(), newSize);
    qDebug() << "New data copied to memory-mapped region.";

    // Ensure that any remaining data in the memory-mapped file is zeroed out if the file was shrunk
    if (newSize < m_fileSize) {
        memset(m_mmap + newSize, 0, m_fileSize - newSize);
        qDebug() << "Remaining memory-mapped region zeroed out.";
    }

    qDebug() << "applyChangesToMemory completed successfully.";
}
