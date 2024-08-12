#include "document.h"
#include "codeeditor.h"  // Updated to match the new filename
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QVBoxLayout>

Document::Document(const QString &filePath, QWidget *parent)
    : QWidget(parent), m_filePath(filePath), m_content("") {
    editor = new CodeEditor(this); // Initialize the CodeEditor
    QVBoxLayout *layout = new QVBoxLayout(this); // Create a vertical layout
    layout->addWidget(editor); // Add the editor to the layout
    setLayout(layout); // Set the layout for this Document widget

    if (!filePath.isEmpty()) {
        openFile(filePath); // Open the file if a path is provided
    }
}

// Getter for m_filePath
QString Document::filePath() const {
    return m_filePath;
}

// Setter for m_filePath
void Document::setFilePath(const QString &path) {
    m_filePath = path;
}

// Getter for m_content
QString Document::content() const {
    return m_content;
}

// Setter for m_content
void Document::setContent(const QString &content) {
    m_content = content;
    editor->setPlainText(content); // Update the editor with the new content
}

void Document::openFile(const QString &filePath) {
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        m_content = in.readAll();
        editor->setPlainText(m_content); // Set the editor's content
        m_filePath = filePath; // Set file path
    } else {
        QMessageBox::warning(this, tr("Error"), tr("Cannot open file: ") + file.errorString());
    }
}

void Document::saveFile() {
    if (m_filePath.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("No file path associated with this document."));
        return; // No path to save
    }

    QFile file(m_filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << editor->toPlainText(); // Get text from editor
        file.close();
        m_content = editor->toPlainText(); // Update m_content
    } else {
        QMessageBox::warning(this, tr("Error"), tr("Cannot save file: ") + file.errorString());
    }
}

void Document::saveFileAs(const QString &newFilePath) {
    m_filePath = newFilePath; // Update the current file path
    saveFile(); // Call saveFile to handle the saving
}
