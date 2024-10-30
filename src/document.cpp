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
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QtConcurrent/QtConcurrent>
#include <QFileInfo>
#include "document.h"
#include "fileloaderworker.h"
#include "codeeditor.h"
#include "languages/languagemanager.h"

Document::Document(const QString &filePath, QWidget *parent)
    : QWidget(parent), m_totalBytesRead(0), m_workerThread(new QThread(this)), m_filePath(filePath), m_isModified(false) {

    qDebug() << "Document created for file: " << filePath;
    // Initialize the code editor and layout
    m_editor = new CodeEditor(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_editor);
    setLayout(layout);

    QFileInfo fileInfo(m_filePath);
    m_fileName = fileInfo.fileName();
    // Initialize status label and progress bar
    m_statusLabel = new QLabel("", this);
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    layout->addWidget(m_statusLabel);
    layout->addWidget(m_progressBar);
    m_statusLabel->setVisible(true);
    m_progressBar->setVisible(false);

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
    connect(m_fileLoaderWorker, &FileLoaderWorker::loadingFinished, this, &Document::handleProgressBarHiding, Qt::QueuedConnection);
    connect(m_fileLoaderWorker, &FileLoaderWorker::savingFinished, this, &Document::onSavingFinished, Qt::QueuedConnection);
    connect(this, &Document::savingProgress, m_progressBar, &QProgressBar::setValue, Qt::QueuedConnection);
    connect(this, &Document::uiReady, m_fileLoaderWorker, &FileLoaderWorker::startLoading);
    connect(m_fileLoaderWorker, &FileLoaderWorker::loadingFinished, this, &Document::handleProgressBarHiding);
    // Connect the editor's textChanged signal, but ensure changes are only registered after loading is complete
    connect(m_editor, &QPlainTextEdit::textChanged, this, [this]() {
        if (!m_isLoading) {
            qDebug() << "Text changed for document: " << m_filePath << ", setting modified flag.";
            this->setModified(true);
        }
    });
    connect(m_editor, &QPlainTextEdit::cursorPositionChanged, this, [this]() {
        setSavedCursorPosition(m_editor->textCursor().position());
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

QString Document::filePath() const {
    return m_filePath;
}

void Document::onLoadingStarted() {
    qDebug() << "Loading started for document:" << m_filePath;

    // Reset progress and bytes tracking
    m_totalBytesInserted = 0;
    m_totalBytesRead = 0;
    m_lastSmoothedProgress = 0;

    // Set the label text and make widgets visible
    m_statusLabel->setText("Loading File...");
    m_statusLabel->setVisible(true);
    m_progressBar->setValue(0);
    m_progressBar->setVisible(true);
}

void Document::onSavingStarted() {
    qDebug() << "Saving started for document:" << m_filePath;
    m_isSaving = true;
    m_statusLabel->setText("Saving File...");
    m_statusLabel->setVisible(true);
    m_progressBar->setValue(0);
    m_progressBar->setVisible(true);
}

void Document::onLoadingProgress(int progress) {
    if (progress >= 0 && progress <= 100) {
        m_progressBar->setValue(progress);
        qDebug() << "Progress updated to:" << progress << "%";
    }

    if (progress == 100) {
        qDebug() << "File loading complete.";
        onLoadingFinished();  // Trigger finished logic
    }
}

void Document::onSavingProgress(int progress) {
    qDebug() << "Saving progress updated:" << progress;
    m_progressBar->setValue(progress);

    if (progress == 100) {
        qDebug() << "File saving complete.";
        onSavingFinished();
        emit hideProgressBar();
    }
}

void Document::handleProgressBarHiding() {
    qDebug() << "Hiding progress bar and clearing status label.";
    m_progressBar->setVisible(false);
    m_statusLabel->clear();
    m_statusLabel->setVisible(false);
}

void Document::onLoadingFinished() {
    if (m_progressBar->value() < 100) {
        // Simulate smooth completion if it finishes too fast
        for (int i = m_progressBar->value(); i <= 100; ++i) {
            QThread::msleep(10);  // Gradually update progress to 100%
            m_progressBar->setValue(i);
            QCoreApplication::processEvents();
        }
    }

    qDebug() << "Loading finished for document:" << m_filePath;

    m_progressBar->setVisible(false);  // Hide the progress bar
    m_statusLabel->setVisible(false);  // Hide the status label
}

void Document::onSavingFinished() {
    qDebug() << "Saving finished for document:" << m_filePath;
    m_isSaving = false;
    m_statusLabel->clear();
    m_statusLabel->setVisible(false);
    m_progressBar->setVisible(false);
}

void Document::onLoadingError(const QString &error) {
    qDebug() << "Error loading document:" << m_filePath << " Error:" << error;
    m_statusLabel->setText("Error: " + error);
}

QThread* Document::workerThread() const {
    return m_workerThread;
}

void Document::setFilePath(const QString &path) {
    m_filePath = path;
    m_fileExtension = QFileInfo(m_filePath).suffix();
    qDebug() << "File path set to:" << m_filePath;
}

QString Document::fileName() const {
    return m_fileName;
}

QString Document::getLanguage() const {
    return m_language;
}

QString Document::getEditorContent() const {
    return m_editor->toPlainText();  // Safely access the editor content
}

bool Document::isModified() const {
    qDebug() << "Checking if document is modified for file: " << m_filePath << " - Modified: " << m_isModified;
    return m_isModified;
}

void Document::setModified(bool modified) {
    if (m_isModified != modified) {
        m_isModified = modified;
        emit editor()->modificationChanged(modified);
        qDebug() << "Document modification state changed for" << filePath() << "to:" << modified;
    }
}

int Document::savedCursorPosition() const {
    return m_savedCursorPosition;
}

void Document::setSavedCursorPosition(int position) {
    m_savedCursorPosition = position;
}

void Document::startLoading() {
    m_isLoading = true;  // Document is in loading state
}

void Document::finishLoading() {
    m_isLoading = false;  // Loading is done
}

void Document::saveFile() {
    if (!m_fileLoaderWorker) {
        qDebug() << "No file loader worker initialized. Cannot save file.";
        return;
    }

    QString filePath = m_filePath;
    QString currentText = m_editor->toPlainText();

    if (filePath.isEmpty()) {
        qDebug() << "File path is empty. Save operation aborted.";
        return;
    }

    // Reset progress bar and show it
    m_progressBar->setValue(0);
    m_progressBar->setVisible(true);
    m_statusLabel->setText("Saving File...");
    m_statusLabel->setVisible(true);

    // Start the save operation in the worker thread
    m_isSaving = true;
    m_fileLoaderWorker->saveFile(filePath, currentText);
    m_isModified = false;
    m_editor->document()->setModified(false);
    qDebug() << "Save operation started for file:" << filePath;
}

void Document::saveFileAs(const QString &newFilePath) {
    qDebug() << "Saving file as: " << newFilePath;

    m_filePath = newFilePath;
    m_fileName = QFileInfo(newFilePath).fileName();

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

    QString currentText = m_editor->toPlainText();

    // Reset progress bar and show it
    m_progressBar->setValue(0);
    m_progressBar->setVisible(true);
    m_statusLabel->setText("Saving Copy of File...");

    // Start the save operation in the worker thread
    m_isSaving = true;
    //m_fileLoaderWorker->saveFile(filePath, currentText);

    QFuture<void> future = QtConcurrent::run([this, filePath, currentText]() {
        m_fileLoaderWorker->saveFile(filePath, currentText);
    });

    m_isModified = false;
    m_editor->document()->setModified(false);
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
        reply = QMessageBox::warning(
            this,
            tr(QString("Unsaved Changes for \"%1\"").arg(m_fileName).toUtf8().constData()),
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
        QTextCursor cursor = m_editor->textCursor();
        cursor.movePosition(QTextCursor::Start);

        for (int i = 1; i < lineNumber; ++i) {
            cursor.movePosition(QTextCursor::Down);

            if (cursor.atEnd()) {
                QMessageBox::warning(this, tr("Line Number Out of Bounds"),
                                     tr("The specified line number is outside the bounds of the file."));
                return;
            }
        }

        m_editor->setTextCursor(cursor);
        m_editor->ensureCursorVisible();
        qDebug() << "Moved to line number:" << lineNumber;
    } else {
        qDebug() << "User canceled the line number input.";
    }
}

void Document::goToLineNumberInText(QWidget* parent) {
    bool ok;
    int lineNumber = QInputDialog::getInt(parent, tr("Go to Line"),
                                          tr("Line number:"), 1, 1, INT_MAX, 1, &ok);
    if (ok) {
        QTextCursor cursor = m_editor->textCursor();
        cursor.movePosition(QTextCursor::Start);

        int currentLineNumber = 0;
        while (currentLineNumber < lineNumber - 1 && !cursor.atEnd()) {
            cursor.movePosition(QTextCursor::NextBlock);
            ++currentLineNumber;
        }

        m_editor->setTextCursor(cursor);
        m_editor->ensureCursorVisible();
        qDebug() << "Moved to line number:" << lineNumber;
    } else {
        qDebug() << "User canceled the line number input.";
    }
}

void Document::applySyntaxHighlighter(const QString &language) {
    qDebug() << "Applying syntax highlighter for language: " << language;
    m_language = language;

    if (syntaxHighlighter) {
        qDebug() << "Deleting existing syntax highlighter";
        syntaxHighlighter.reset();  // Safely reset unique_ptr
    }

    // Create highlighter based on the language
    syntaxHighlighter = std::unique_ptr<QSyntaxHighlighter>(LanguageManager::createHighlighterForExtension(language, editor()->document()));
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
        qDebug() << "File path is empty. Save operation aborted!";
        return;
    }

    QString fileContent = m_editor->toPlainText();  // Get the current content from the editor

    // Call the worker to save the file content
    m_fileLoaderWorker->saveFile(filePath, fileContent);
    m_editor->document()->setModified(false);
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
    m_editor->blockSignals(true);
    m_editor->insertPlainText(chunk);
    m_editor->blockSignals(false);

    m_totalBytesRead += chunk.size();

    // Check if loading is complete
    if (m_totalBytesRead >= m_fileSize) {
        qDebug() << "Document loading finished for: " << m_filePath;
        m_isLoading = false;  // Mark loading as complete
        setModified(false);  // Reset the modified flag
        emit loadingFinished();
    }
}

bool Document::isLoading() const {
    return m_isLoading;
}
