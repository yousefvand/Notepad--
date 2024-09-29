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
#include "document.h"
#include "fileloaderworker.h"
#include "codeeditor.h"

Document::Document(const QString &filePath, QWidget *parent)
    : QWidget(parent), m_filePath(filePath), m_totalBytesRead(0), m_workerThread(new QThread(this)) {

    // Initialize the code editor and layout
    editor = new CodeEditor(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(editor);
    setLayout(layout);

    // Initialize status label and progress bar
    m_statusLabel = new QLabel(this);
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    layout->addWidget(m_statusLabel);
    layout->addWidget(m_progressBar);

    // Initialize the file loader worker and move it to the worker thread
    m_fileLoaderWorker = new FileLoaderWorker(m_filePath, this);
    m_fileLoaderWorker->moveToThread(m_workerThread);

    // Connect worker signals
    connect(m_fileLoaderWorker, &FileLoaderWorker::loadingStarted, this, &Document::onLoadingStarted);
    connect(m_fileLoaderWorker, &FileLoaderWorker::loadingProgress, this, &Document::onLoadingProgress);
    connect(m_fileLoaderWorker, &FileLoaderWorker::errorOccurred, this, &Document::onLoadingError);
    connect(m_fileLoaderWorker, &FileLoaderWorker::contentLoaded, this, &Document::insertContentIntoEditor);
    connect(m_fileLoaderWorker, &FileLoaderWorker::contentLoaded, this, &Document::onContentLoaded);
    connect(m_fileLoaderWorker, &FileLoaderWorker::loadingProgress, this, &Document::onLoadingProgress, Qt::UniqueConnection);
    connect(m_fileLoaderWorker, &FileLoaderWorker::fileSizeDetermined, this, &Document::onFileSizeDetermined);

    connect(m_fileLoaderWorker, &FileLoaderWorker::savingStarted, this, &Document::onSavingStarted);
    connect(m_fileLoaderWorker, &FileLoaderWorker::savingProgress, this, &Document::onSavingProgress);
    connect(m_fileLoaderWorker, &FileLoaderWorker::savingFinished, this, &Document::onSavingFinished);
    connect(this, &Document::savingProgress, m_progressBar, &QProgressBar::setValue);

    connect(m_fileLoaderWorker, &FileLoaderWorker::loadingProgress, this, [this](int progress) {
        QMetaObject::invokeMethod(m_progressBar, "setValue", Qt::QueuedConnection, Q_ARG(int, progress));
    });

    connect(m_fileLoaderWorker, &FileLoaderWorker::loadingFinished, this, &Document::onLoadingFinished);
    connect(m_fileLoaderWorker, &FileLoaderWorker::savingFinished, this, &Document::onSavingFinished);

    // Start worker thread
    m_workerThread->start();
    qDebug() << "Worker thread running:" << m_workerThread->isRunning();

    // Connect uiReady signal to start loading in the worker thread
    connect(this, &Document::uiReady, m_fileLoaderWorker, &FileLoaderWorker::startLoading);

    // Use QTimer::singleShot to emit uiReady signal after a delay
    QTimer::singleShot(100, this, [this]() {
        qDebug() << "Emitting uiReady signal after ensuring UI is ready...";
        emit uiReady();
    });

    // Set up status label and progress bar
    m_statusLabel->setText("Loading File...");
    m_progressBar->setVisible(true);
    // Connect signals for progress updates
    connect(this, &Document::hideProgressBar, this, [this]() {
        m_progressBar->setVisible(false);  // Hide progress bar when loading/saving is done
        m_statusLabel->setText("");
    });

    qDebug() << "Document and thread initialized for file:" << m_filePath;
}

Document::~Document() {
    // Safely disconnect signals before cleaning up resources
    if (m_fileLoaderWorker) {
        qDebug() << "Disconnecting signals for m_fileLoaderWorker";
        disconnect(m_fileLoaderWorker, nullptr, this, nullptr);  // Disconnect all signals from worker
    }

    if (m_workerThread) {
        if (m_workerThread->isRunning()) {
            qDebug() << "Stopping worker thread";
            m_workerThread->quit();  // Gracefully stop the worker thread
            m_workerThread->wait();  // Ensure the thread finishes its work
        }
        m_workerThread->deleteLater();  // Schedule thread deletion
        qDebug() << "Worker thread cleaned up.";
    }

    if (m_fileLoaderWorker) {
        qDebug() << "Deleting m_fileLoaderWorker";
        m_fileLoaderWorker->deleteLater();  // Safely delete the file loader worker
        m_fileLoaderWorker = nullptr;  // Nullify the pointer to avoid double deletion
    }

    qDebug() << "Document destructor called, all resources cleaned up.";
}

void Document::onLoadingStarted() {
    qDebug() << "Loading started for document:" << m_filePath;
    m_statusLabel->setText("Loading File...");
    m_progressBar->setValue(0);
    m_progressBar->setVisible(true);
    m_statusLabel->setVisible(true);
}

void Document::onSavingStarted() {
    m_isSaving = true;
    m_statusLabel->setText("Saving File...");
    m_progressBar->setVisible(true);
    m_progressBar->setValue(0);
}

void Document::onLoadingProgress(int progress) {
    if (progress - m_lastSmoothedProgress >= m_smoothProgressUpdateInterval || progress == 100) {
        m_lastSmoothedProgress = progress;
        m_progressBar->setValue(progress);

        qDebug() << "Smooth progress updated to:" << progress;
    }

    if (progress == 100) {
        emit hideProgressBar();
    }
}

void Document::onSavingProgress(int progress) {
    qDebug() << "Saving progress updated:" << progress;
    m_progressBar->setValue(progress);

    if (progress == 100) {
        emit hideProgressBar();
    }
}

void Document::onLoadingFinished() {
    m_isLoading = false;
    emit loadingProgress(100);
    emit hideProgressBar();     // Hide the progress bar when loading finishes
    qDebug() << "Loading finished, m_isLoading = " << m_isLoading;
}

void Document::onSavingFinished() {
    m_isSaving = false;
    m_statusLabel->setText("");  // Clear the label after saving
    m_progressBar->setVisible(false);  // Hide the progress bar when saving is finished
}

void Document::onLoadingError(const QString &error) {
    qDebug() << "Error loading document:" << m_filePath << " Error:" << error;
    m_statusLabel->setText("Error: " + error);
}

void Document::insertContentIntoEditor(const QString &content) {
    if (m_totalBytesRead == 0) {
        qDebug() << "Inserting initial content into the editor...";
        editor->clear();  // Clear existing content if any
    }
    editor->insertPlainText(content);
    m_originalText = content;  // Save the original content for comparison
}

void Document::setFilePath(const QString &path) {
    m_filePath = path;
    qDebug() << "File path set to:" << m_filePath;
}

QString Document::filePath() const {
    return m_filePath;
}

void Document::openFile(const QString &filePath) {
    m_filePath = filePath;
    qDebug() << "Opening file:" << m_filePath;

    qDebug() << "About to emit uiReady signal from openFile...";
    // Emit the signal to start loading the file in the worker thread
    emit uiReady();
}

void Document::saveFile() {
    if (!m_fileLoaderWorker) {
        qDebug() << "No file loader worker initialized. Cannot save file.";
        return;
    }

    QString filePath = m_filePath;
    QString currentText = editor->toPlainText();

    if (filePath.isEmpty()) {
        qDebug() << "File path is empty. Save operation aborted.";
        return;
    }

    // Reset progress bar and show it
    m_progressBar->setValue(0);
    m_progressBar->setVisible(true);
    m_statusLabel->setText("Saving File...");

    // Start the save operation in the worker thread
    m_isSaving = true;
    m_fileLoaderWorker->saveFile(filePath, currentText);
    qDebug() << "Save operation started for file:" << filePath;
}

void Document::saveFileAs(const QString &newFilePath) {
    qDebug() << "Saving file as:" << newFilePath;

    // Set the new file path
    m_filePath = newFilePath;

    // Now call saveFile to save the content to the new file path
    saveFile();
}

bool Document::checkForUnsavedChanges() {
    QString currentText = editor->toPlainText();  // Get the current text in the editor
    return !compareText(currentText, m_originalText);  // Compare current text with the original text
}

bool Document::closeDocument() {
    qDebug() << "Checking document close: isLoading=" << m_isLoading << ", isSaving=" << m_isSaving;
    if (m_isLoading || m_isSaving) {
        qDebug() << "Document still loading or saving, cannot close.";
        return false;
    }
    // Proceed with closing the document
    return true;
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
        qDebug() << "Moved to line number:" << lineNumber;
    } else {
        qDebug() << "User canceled the line number input.";
    }
}

bool Document::promptForSave() {
    // Your implementation
    QMessageBox::StandardButton reply;
    reply = QMessageBox::warning(this, tr("Unsaved Changes"),
                                 tr("You have unsaved changes."),
                                 QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (reply == QMessageBox::Save) {
        saveFile();  // Assuming saveFile() exists in the class
        return true;
    } else if (reply == QMessageBox::Discard) {
        return true;
    } else {
        return false;  // Cancel closing
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
        editor->ensureCursorVisible();
        qDebug() << "Moved to line number:" << lineNumber;
    } else {
        qDebug() << "User canceled the line number input.";
    }
}

void Document::applySyntaxHighlighter(const QString &language) {
    qDebug() << "Applying syntax highlighter for language:" << language;

    if (syntaxHighlighter) {
        delete syntaxHighlighter;
        syntaxHighlighter = nullptr;
    }

    // Assuming LanguageManager handles creating syntax highlighters for specific languages
    syntaxHighlighter = LanguageManager::createHighlighterForExtension(language, editor->document());

    if (syntaxHighlighter) {
        syntaxHighlighter->rehighlight();
        qDebug() << "Syntax highlighter applied for language:" << language;
    } else {
        qDebug() << "No syntax highlighter found for language:" << language;
    }
}

bool Document::compareText(const QString &text1, const QString &text2) {

    qDebug() << "text1 (loaded from editor) length: " << text1.length();
    qDebug() << "text2 (from file) length: " << text2.length();

    // First compare lengths
    if (text1.length() != text2.length()) {
        qDebug() << "text1 and text2 are different in length";
        return false;
    }

    // Then compare character by character
    for (int i = 0; i < text1.length(); ++i) {
        if (text1[i] != text2[i]) {
            qDebug() << "text1 and text2 are different in content at position:" << i;
            return false;
        }
    }

    return true;
}

void Document::saveDocument() {
    QString filePath = m_filePath;  // Get the file path
    QString fileContent = editor->toPlainText();  // Get the current content from the editor

    if (filePath.isEmpty()) {
        qDebug() << "File path is empty. Save operation aborted.";
        return;
    }

    // Call the worker to save the file content
    m_fileLoaderWorker->saveFile(filePath, fileContent);
}

void Document::onFileSizeDetermined(qint64 fileSize) {
    m_fileSize = fileSize;
    qDebug() << "File size set in Document: " << m_fileSize;
}

void Document::onContentLoaded(const QString &chunk) {
    if (m_fileSize <= 0) {
        qDebug() << "onContentLoaded: Invalid file size!";
        return;
    }

    m_totalBytesRead += chunk.size();

    // Buffer more content before updating the editor
    m_bufferedContent += chunk;

    // Calculate a dynamic buffer size (e.g., 1% of file size or 1MB minimum)
    qint64 dynamicBufferSize = std::max(m_fileSize / 100, static_cast<qint64>(1 * 1024 * 1024));

    if (m_bufferedContent.size() >= dynamicBufferSize) {
        editor->moveCursor(QTextCursor::End);
        qDebug() << "Buffered content size before inserting: " << m_bufferedContent.size();

        // Insert content with proper line breaks
        editor->insertPlainText(m_bufferedContent);
        m_bufferedContent.clear();  // Clear buffer after insertion

        QCoreApplication::processEvents();  // Keep UI responsive
    }

    // Update progress bar
    emit loadingProgress(static_cast<int>((m_totalBytesRead * 100) / m_fileSize));

    // Handle loading finished
    if (m_totalBytesRead >= m_fileSize) {
        emit loadingFinished();

        // Insert remaining buffered content
        if (!m_bufferedContent.isEmpty()) {
            editor->moveCursor(QTextCursor::End);
            editor->insertPlainText(m_bufferedContent);
            m_bufferedContent.clear();
        }

        // Move cursor to the first line after loading completes
        editor->moveCursor(QTextCursor::Start);

        // Clear status label after loading completes
        m_statusLabel->setText("");
    }
}
