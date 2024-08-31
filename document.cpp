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
    connect(editor->verticalScrollBar(), &QScrollBar::valueChanged, this, &Document::trackChanges);
    connect(editor, &QPlainTextEdit::cursorPositionChanged, this, &Document::trackChanges);
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

void Document::openFile(const QString &filePath) {
    m_filePath = filePath;

    // Reset the editor before loading a new file
    editor->clear();

    // Emit signals to update the UI before loading
    emit updateStatusMessage("Loading File...");
    emit updateProgress(0);

    // Determine the language based on file extension and apply the syntax highlighter
    QString language = LanguageManager::getLanguageFromExtension(QFileInfo(filePath).suffix());
    applySyntaxHighlighter(language);

    QtConcurrent::run([this, filePath]() {
        int fd = ::open(filePath.toStdString().c_str(), O_RDONLY);
        if (fd == -1) {
            QMetaObject::invokeMethod(this, [this]() {
                QMessageBox::warning(this, tr("Error"), tr("Cannot open file: ") + m_filePath);
            });
            return;
        }

        m_fileSize = lseek(fd, 0, SEEK_END);
        if (m_fileSize == -1) {
            ::close(fd);
            QMetaObject::invokeMethod(this, [this]() {
                QMessageBox::warning(this, tr("Error"), tr("Cannot determine file size: ") + m_filePath);
            });
            return;
        }
        lseek(fd, 0, SEEK_SET);

        m_mmap = static_cast<char*>(mmap(nullptr, m_fileSize, PROT_READ, MAP_PRIVATE, fd, 0));
        if (m_mmap == MAP_FAILED) {
            ::close(fd);
            QMetaObject::invokeMethod(this, [this]() {
                QMessageBox::warning(this, tr("Error"), tr("Cannot map file to memory: ") + m_filePath);
            });
            return;
        }

        ::close(fd);

        qint64 bytesRead = 0;
        const qint64 chunkSize = 1024 * 1024; // Read in 1MB chunks

        while (bytesRead < m_fileSize) {
            qint64 bytesToRead = qMin(chunkSize, m_fileSize - bytesRead);

            QString textChunk = QString::fromUtf8(m_mmap + bytesRead, bytesToRead);

            QMetaObject::invokeMethod(this, [this, textChunk]() {
                QTextCursor cursor(editor->document());
                cursor.movePosition(QTextCursor::End);
                cursor.insertText(textChunk);
            });

            bytesRead += bytesToRead;

            int progress = static_cast<int>((static_cast<double>(bytesRead) / m_fileSize) * 100);
            QMetaObject::invokeMethod(this, [this, progress]() {
                emit updateProgress(progress);
            });

            // Optional: Small delay to prevent UI lockup (if needed)
            QThread::msleep(1);
        }

        QMetaObject::invokeMethod(this, [this]() {
            QTextCursor cursor(editor->document());
            cursor.movePosition(QTextCursor::Start);
            editor->setTextCursor(cursor);
            editor->ensureCursorVisible();

            emit updateProgress(100);
            emit updateStatusMessage("");
            qDebug() << "Loaded content into editor. Document size:" << editor->document()->characterCount();
        });
    });
}

void Document::loadContent() {
    if (m_mmap) {
        editor->setPlainText(QString::fromUtf8(m_mmap, m_fileSize));
    } else {
        qDebug() << "No memory-mapped file available to load.";
    }
}

void Document::trackChanges() {
    // Track changes implementation remains the same
}

void Document::saveFile() {
    emit updateStatusMessage("Saving File...");
    emit updateProgress(0);

    QtConcurrent::run([this]() {
        QFile file(m_filePath);
        if (!file.open(QIODevice::WriteOnly)) {
            QMetaObject::invokeMethod(this, [this]() {
                emit updateStatusMessage("Error Saving File");
                emit updateProgress(0);
            });
            return;
        }

        QDataStream out(&file);

        qint64 bytesWritten = 0;
        const qint64 chunkSize = 1024 * 1024; // Write in 1MB chunks

        while (bytesWritten < m_fileSize) {
            qint64 bytesToWrite = qMin(chunkSize, m_fileSize - bytesWritten);

            out.writeRawData(m_mmap + bytesWritten, bytesToWrite);

            bytesWritten += bytesToWrite;

            int progress = static_cast<int>((static_cast<double>(bytesWritten) / m_fileSize) * 100);
            QMetaObject::invokeMethod(this, [this, progress]() {
                emit updateProgress(progress);
            });
        }

        file.close();

        QMetaObject::invokeMethod(this, [this]() {
            emit updateProgress(100);
            emit updateStatusMessage(""); // Clear the status message after saving
        });
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
