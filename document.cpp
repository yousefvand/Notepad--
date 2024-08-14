#include "document.h"
#include "codeeditor.h"
#include "cppsyntaxhighlighter.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QCryptographicHash>
#include <QScrollBar>
#include <QDebug>
#include <QFileDialog>
#include <QThread>
#include <QFuture>
#include <QtConcurrent>

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

QString Document::filePath() const {
    return m_filePath;
}

void Document::setFilePath(const QString &path) {
    m_filePath = path;
    m_fileExtension = QFileInfo(path).suffix();
    applySyntaxHighlighter();
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
    if (newFilePath.isEmpty()) return;
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
    QByteArray currentHash = calculateModifiedMD5();

    if (currentHash != m_originalHash) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::warning(this, tr("Unsaved Changes"),
                                     tr("You have unsaved changes."),
                                     QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (reply == QMessageBox::Save) {
            saveFile();
            return true;
        } else if (reply == QMessageBox::Discard) {
            return true; // Allow closing without saving
        } else {
            return false; // Cancel closing
        }
    }

    return true; // No unsaved changes, allow closing
}

void Document::applySyntaxHighlighter() {
    if (syntaxHighlighter) {
        delete syntaxHighlighter;
        syntaxHighlighter = nullptr;
    }

    if (m_fileExtension == "cpp" || m_fileExtension == "cxx") {
        syntaxHighlighter = new CppSyntaxHighlighter(editor->document());
    }
}
