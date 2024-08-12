#include "document.h"
#include "codeeditor.h"
#include "cppsyntaxhighlighter.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QFileInfo>

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
        QMessageBox::warning(this, tr("Error"), tr("No file path associated with this document."));
        return;
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
    m_filePath = newFilePath;
    saveFile();
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
