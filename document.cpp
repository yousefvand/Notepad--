#include "document.h"
#include "codeeditor.h"
#include "cppsyntaxhighlighter.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QCryptographicHash>
#include <QFileDialog> // Include QFileDialog for Save As dialog

Document::Document(const QString &filePath, QWidget *parent)
    : QWidget(parent), m_filePath(filePath), m_content(""), syntaxHighlighter(nullptr) {
    editor = new CodeEditor(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(editor);
    setLayout(layout);

    if (!filePath.isEmpty()) {
        openFile(filePath);
    }
}

QString Document::filePath() const {
    return m_filePath;
}

void Document::setFilePath(const QString &path) {
    m_filePath = path;
    m_fileExtension = QFileInfo(path).suffix();
    applySyntaxHighlighter();
}

QString Document::content() const {
    return m_content;
}

void Document::setContent(const QString &content) {
    m_content = content;
    editor->setPlainText(content);
}

QString Document::fileExtension() const {
    return m_fileExtension;
}

void Document::openFile(const QString &filePath) {
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        m_content = in.readAll();
        editor->setPlainText(m_content);
        m_filePath = filePath;
        setFilePath(filePath); // Update file extension and apply syntax highlighter
    } else {
        QMessageBox::warning(this, tr("Error"), tr("Cannot open file: ") + file.errorString());
    }
}

void Document::saveFile() {
    if (m_filePath.isEmpty()) {
        // If no file path is associated, prompt user with Save As dialog
        saveFileAs(QFileDialog::getSaveFileName(this, tr("Save File As"), "", tr("Text Files (*.txt);;All Files (*)")));
        return; // Exit to avoid further processing
    }

    QFile file(m_filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << editor->toPlainText();
        file.close();
        m_content = editor->toPlainText();
    } else {
        QMessageBox::warning(this, tr("Error"), tr("Cannot save file: ") + file.errorString());
    }
}

void Document::saveFileAs(const QString &newFilePath) {
    if (newFilePath.isEmpty()) return; // Don't proceed if no file path is provided
    m_filePath = newFilePath; // Update the file path
    saveFile(); // Call saveFile to actually perform the saving
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

QByteArray Document::calculateMD5(const QString &data) {
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(data.toUtf8());
    return hash.result();
}

bool Document::closeDocument() {
    QString currentContent = editor->toPlainText();
    QString fileContent;

    // Read current file content if file path is set
    if (!m_filePath.isEmpty()) {
        QFile file(m_filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            fileContent = in.readAll();
            file.close();
        }
    }

    // Compare MD5 hashes
    QByteArray currentHash = calculateMD5(currentContent);
    QByteArray fileHash = calculateMD5(fileContent);

    // Check if content has changed
    if (currentHash != fileHash) {
        // Show modal dialog
        QMessageBox::StandardButton reply;
        reply = QMessageBox::warning(this, tr("Unsaved Changes"),
                                     tr("You have unsaved changes."),
                                     QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (reply == QMessageBox::Save) {
            saveFile(); // Save changes (will handle path through saveFile logic)
            return true; // Allow closing
        } else if (reply == QMessageBox::Discard) {
            return true; // Allow closing without saving
        } else {
            return false; // Cancel closing
        }
    }

    return true; // No unsaved changes, allow closing
}
