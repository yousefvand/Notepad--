#include "document.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileInfo>
#include <QVBoxLayout>

Document::Document(QWidget *parent) : QWidget(parent), filePath("") {
    editor = new CodeEditor(this); // Initialize the CodeEditor
    QVBoxLayout *layout = new QVBoxLayout(this); // Create a vertical layout
    layout->addWidget(editor); // Add the editor to the layout
    setLayout(layout); // Set the layout for this Document widget
}

Document::Document(const QString &filePath, QWidget *parent) : QWidget(parent), filePath(filePath) {
    editor = new CodeEditor(this); // Initialize the CodeEditor
    QVBoxLayout *layout = new QVBoxLayout(this); // Create a vertical layout
    layout->addWidget(editor); // Add the editor to the layout
    setLayout(layout); // Set the layout for this Document widget
    openFile(filePath); // Open the file
}

void Document::openFile(const QString &filePath) {
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString content = in.readAll();
        editor->setPlainText(content); // Set the editor's content
        this->filePath = filePath; // Set file path
    } else {
        QMessageBox::warning(this, tr("Error"), tr("Cannot open file: ") + file.errorString());
    }
}

void Document::saveFile() {
    if (filePath.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("No file path associated with this document."));
        return; // No path to save
    }

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << editor->toPlainText(); // Get text from editor
        file.close();
    } else {
        QMessageBox::warning(this, tr("Error"), tr("Cannot save file: ") + file.errorString());
    }
}

void Document::saveFileAs(const QString &newFilePath) {
    filePath = newFilePath; // Update the current file path
    saveFile(); // Call saveFile to handle the saving
}
