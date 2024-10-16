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
    : QWidget(parent), m_filePath(filePath), m_isModified(false), m_totalBytesRead(0), m_workerThread(new QThread(this)) {

    qDebug() << "Document created for file: " << filePath;
    // Initialize the code editor and layout
    editor = new CodeEditor(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(editor);
    setLayout(layout);

    // Initialize status label and progress bar
    m_statusLabel = new QLabel(this);
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setVisible(false);
    layout->addWidget(m_statusLabel);
    layout->addWidget(m_progressBar);

    // Initialize the file loader worker and move it to the worker thread
    m_fileLoaderWorker = new FileLoaderWorker(m_filePath, this);
    m_fileLoaderWorker->moveToThread(m_workerThread);

    connect(m_fileLoaderWorker, &FileLoaderWorker::loadingStarted, this, &Document::onLoadingStarted, Qt::QueuedConnection);
    connect(m_fileLoaderWorker, &FileLoaderWorker::errorOccurred, this, &Document::onLoadingError, Qt::QueuedConnection);
    connect(m_fileLoaderWorker, &FileLoaderWorker::contentLoaded, this, &Document::onContentLoaded, Qt::QueuedConnection);
    connect(m_fileLoaderWorker, &FileLoaderWorker::fileSizeDetermined, this, &Document::onFileSizeDetermined, Qt::QueuedConnection);
    connect(m_fileLoaderWorker, &FileLoaderWorker::savingStarted, this, &Document::onSavingStarted, Qt::QueuedConnection);
    connect(m_fileLoaderWorker, &FileLoaderWorker::savingProgress, this, &Document::onSavingProgress, Qt::QueuedConnection);
    connect(m_fileLoaderWorker, &FileLoaderWorker::savingFinished, this, &Document::onSavingFinished, Qt::QueuedConnection);
    connect(m_fileLoaderWorker, &FileLoaderWorker::loadingProgress, this, &Document::onLoadingProgress, Qt::QueuedConnection);
    connect(m_fileLoaderWorker, &FileLoaderWorker::loadingFinished, this, &Document::onLoadingFinished, Qt::QueuedConnection);
    connect(m_fileLoaderWorker, &FileLoaderWorker::savingFinished, this, &Document::onSavingFinished, Qt::QueuedConnection);
    connect(this, &Document::savingProgress, m_progressBar, &QProgressBar::setValue, Qt::QueuedConnection);
    connect(this, &Document::uiReady, m_fileLoaderWorker, &FileLoaderWorker::startLoading);
    connect(this, &Document::hideProgressBar, this, [this]() {
        m_progressBar->setVisible(false);  // Hide progress bar when loading/saving is done
        m_statusLabel->setText("");
    });
    // Connect the editor's textChanged signal, but ensure changes are only registered after loading is complete
    connect(editor, &QPlainTextEdit::textChanged, this, [this]() {
        if (!m_isLoading) {
            qDebug() << "Text changed for document: " << m_filePath << ", setting modified flag.";
            this->setModified(true);
        }
    });

    // Start worker thread
    m_workerThread->start();
    qDebug() << "Worker thread running:" << m_workerThread->isRunning();

    // Use QTimer::singleShot to emit uiReady signal after a delay
    QTimer::singleShot(100, this, [this]() {
        qDebug() << "Emitting uiReady signal after ensuring UI is ready...";
        emit uiReady();
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
    m_totalBytesInserted = 0;  // Reset bytes inserted when loading starts
    m_totalBytesRead = 0;      // Reset bytes read
    m_lastSmoothedProgress = 0; // Reset progress tracking
    qDebug() << "Loading started for document:" << m_filePath;
    m_statusLabel->setText("Loading File...");
    m_progressBar->setValue(0);
    m_progressBar->setVisible(true);
    m_statusLabel->setVisible(true);
}


void Document::onSavingStarted() {
    m_isSaving = true;
    m_statusLabel->setVisible(true);
    m_progressBar->setVisible(true);
    m_progressBar->setValue(0);
}

void Document::onLoadingProgress(int progress) {
    m_statusLabel->setText("Loading File...");
    if (progress - m_lastSmoothedProgress >= m_smoothProgressUpdateInterval || progress == 100) {
        m_lastSmoothedProgress = progress;
        QMetaObject::invokeMethod(m_progressBar, "setValue", Qt::QueuedConnection, Q_ARG(int, progress));
        qDebug() << "Progress bar updated with progress: " << progress;
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
    setModified(false);  // Ensure the document starts as unmodified
    qDebug() << "Document loading finished for: " << m_filePath;

    emit loadingProgress(100);
    emit hideProgressBar();

    // Detect file extension and apply syntax highlighter
    QString fileExtension = QFileInfo(m_filePath).suffix();
    QString language = LanguageManager::getLanguageFromExtension(fileExtension);

    applySyntaxHighlighter(language);

    qDebug() << "File loading finished, syntax highlighter applied.";
}

void Document::onSavingFinished() {
    m_isSaving = false;
    m_statusLabel->setText("");
    m_progressBar->setVisible(false);
}

void Document::onLoadingError(const QString &error) {
    qDebug() << "Error loading document:" << m_filePath << " Error:" << error;
    m_statusLabel->setText("Error: " + error);
}

void Document::setFilePath(const QString &path) {
    m_filePath = path;
    m_fileExtension = QFileInfo(m_filePath).suffix();
    qDebug() << "File path set to:" << m_filePath;
}

QString Document::filePath() const {
    return m_filePath;
}

QString Document::getEditorContent() const {
    return editor->toPlainText();  // Safely access the editor content
}

bool Document::isModified() const {
    qDebug() << "Checking if document is modified for file: " << m_filePath << " - Modified: " << m_isModified;
    return m_isModified;
}

void Document::setModified(bool modified) {
    m_isModified = modified;
    qDebug() << "Document modification state changed for file: " << m_filePath << " to: " << m_isModified;
}

void Document::startLoading() {
    m_isLoading = true;  // Document is in loading state
}

void Document::finishLoading() {
    m_isLoading = false;  // Loading is done
}

void Document::openFile(const QString &filePath) {
    QFile file(filePath);
    m_fileExtension = QFileInfo(filePath).suffix();
    qDebug() << "Opening file:" << filePath;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        originalFileContent = in.readAll();

        m_isLoading = true;  // Set the loading flag

        editor->setPlainText(originalFileContent);  // Set content into editor

        m_isLoading = false;  // Reset loading flag after content is loaded
        setModified(false);  // Mark the document as not modified after loading
        file.close();

        QString language = LanguageManager::getLanguageFromExtension(m_fileExtension);
        applySyntaxHighlighter(language);

        qDebug() << "File loading finished for: " << filePath;
    } else {
        qDebug() << "Error: Could not open file: " << filePath;
    }
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
    m_isModified = false;
    qDebug() << "Save operation started for file:" << filePath;
}

void Document::saveFileAs(const QString &newFilePath) {
    qDebug() << "Saving file as:" << newFilePath;

    // Set the new file path
    m_filePath = newFilePath;
    saveFile();
}

void Document::saveAcopyAs() {
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save a Copy As"), QString(),
                                                    tr("Text Files (*.txt);;All Files (*)"));

    if (filePath.isEmpty())
        return;

    if (!m_fileLoaderWorker) {
        qDebug() << "No file loader worker initialized. Cannot save file.";
        return;
    }

    QString currentText = editor->toPlainText();

    // Reset progress bar and show it
    m_progressBar->setValue(0);
    m_progressBar->setVisible(true);
    m_statusLabel->setText("Saving Copy of File...");

    // Start the save operation in the worker thread
    m_isSaving = true;
    m_fileLoaderWorker->saveFile(filePath, currentText);
    m_isModified = false;
    qDebug() << "Save operation started for file:" << filePath;
}

bool Document::closeDocument() {
    qDebug() << "Checking document close: isLoading=" << m_isLoading << ", isSaving=" << m_isSaving;

    if (m_isLoading || m_isSaving) {
        qDebug() << "Document still loading or saving, cannot close.";
        return false;
    }

    if (isModified()) {
        // Document has unsaved changes, so prompt the user
        QMessageBox::StandardButton reply;
        reply = QMessageBox::warning(this, tr("Unsaved Changes"),
                                     tr("The document has unsaved changes. Do you want to save your changes?"),
                                     QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (reply == QMessageBox::Save) {
            saveFile();  // Save the document
            return true;  // After saving, allow closing the tab
        } else if (reply == QMessageBox::Discard) {
            return true;  // Discard changes and close the document
        } else {
            return false;  // Cancel the closing operation
        }
    }

    // Proceed with closing the document since no unsaved changes
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
    QMessageBox::StandardButton reply;
    reply = QMessageBox::warning(this, tr("Unsaved Changes"),
                                 tr("You have unsaved changes."),
                                 QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (reply == QMessageBox::Save) {
        saveFile();  // Save the file before closing
        return true;  // Proceed with closing the tab
    } else if (reply == QMessageBox::Discard) {
        return true;  // Proceed with closing the tab
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
    qDebug() << "Applying syntax highlighter for language: " << language;

    if (syntaxHighlighter) {
        qDebug() << "Deleting existing syntax highlighter";
        syntaxHighlighter.reset();  // Safely reset unique_ptr
    }

    // Create highlighter based on the language
    syntaxHighlighter = std::unique_ptr<QSyntaxHighlighter>(LanguageManager::createHighlighterForExtension(language, editor->document()));
    if (syntaxHighlighter) {
        qDebug() << "Syntax highlighter created for language: " << language;
        syntaxHighlighter->rehighlight();  // Rehighlight to apply
    } else {
        qDebug() << "Failed to create syntax highlighter for language: " << language;
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

    if (filePath.isEmpty()) {
        qDebug() << "File path is empty. Save operation aborted.";
        return;
    }

    QString fileContent = editor->toPlainText();  // Get the current content from the editor

    // Call the worker to save the file content
    m_fileLoaderWorker->saveFile(filePath, fileContent);
}

void Document::onFileSizeDetermined(qint64 fileSize) {
    m_fileSize = fileSize;
    qDebug() << "File size set in Document: " << m_fileSize;
}

void Document::onContentLoaded(const QString &chunk) {
    if (m_fileSize <= 0) {
        qDebug() << "Error: Invalid file size!";
        return;
    }

    // Block signals during content loading
    editor->blockSignals(true);
    editor->insertPlainText(chunk);
    editor->blockSignals(false);

    m_totalBytesRead += chunk.size();

    // Check if loading is complete
    if (m_totalBytesRead >= m_fileSize) {
        qDebug() << "Document loading finished for: " << m_filePath;
        m_isLoading = false;  // Mark loading as complete
        setModified(false);  // Reset the modified flag
        emit loadingFinished();
    }
}


