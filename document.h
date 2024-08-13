#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QWidget>
#include <QString>
#include <QByteArray>

class CodeEditor;
class CppSyntaxHighlighter;

class Document : public QWidget {
    Q_OBJECT

public:
    explicit Document(const QString &filePath = "", QWidget *parent = nullptr);

    QString filePath() const;
    void setFilePath(const QString &path);

    QString content() const;
    void setContent(const QString &content);

    QString fileExtension() const;

    void openFile(const QString &filePath);
    void saveFile();
    void saveFileAs(const QString &newFilePath);
    bool closeDocument(); // Change made here

private:
    QString m_filePath;
    QString m_content;
    QString m_fileExtension;

    CodeEditor *editor;
    CppSyntaxHighlighter *syntaxHighlighter;

    void applySyntaxHighlighter();

    QByteArray calculateMD5(const QString &data); // New method for calculating MD5
};

#endif // DOCUMENT_H
