#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QWidget>
#include <QString>

class CodeEditor;

class Document : public QWidget {
    Q_OBJECT

public:
    explicit Document(const QString &filePath = "", QWidget *parent = nullptr);

    // Getter for m_filePath
    QString filePath() const;

    // Setter for m_filePath
    void setFilePath(const QString &path);

    // Getter for m_content
    QString content() const;

    // Setter for m_content
    void setContent(const QString &content);

    // Function to open a file
    void openFile(const QString &filePath);

    // Function to save the file
    void saveFile();

    // Function to save the file with a new name
    void saveFileAs(const QString &newFilePath);

private:
    QString m_filePath;
    QString m_content;

    CodeEditor *editor;
};

#endif // DOCUMENT_H
