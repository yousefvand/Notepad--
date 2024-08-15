#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QWidget>
#include <QFile>
#include <QMap>
#include <QCryptographicHash>

class CodeEditor; // Forward declaration
class CppSyntaxHighlighter; // Forward declaration

class Document : public QWidget {
    Q_OBJECT

public:
    Document(const QString &filePath, QWidget *parent = nullptr);
    QString filePath() const;
    void setFilePath(const QString &path);
    void setLanguage(const QString &language);
    QString getLanguage() const;
    void openFile(const QString &filePath);
    void saveFile();
    void saveFileAs(const QString &newFilePath);
    bool closeDocument();
    //void goToLineNumberInText(int lineNumber);
    void goToLineNumberInText(QWidget* parent); // Pass QWidget for the parent window


    // New method to go to a specific line number
    void goToLineNumberInEditor();

signals:
    void saveError(const QString &error); // Signal for save errors
    void saveCompleted(); // Signal for successful save

private:
    void loadContent();
    void updatePointers();
    void trackChanges();
    QByteArray calculateMD5Stream(QFile *file);
    QByteArray calculateModifiedMD5();
    void applySyntaxHighlighter();
    void applyCppFormatting();

    QString m_filePath;
    QString m_fileExtension;
    QFile m_file;
    CodeEditor *editor;
    CppSyntaxHighlighter *syntaxHighlighter;
    qint64 m_fileSize;
    qint64 m_startPointer;
    qint64 m_endPointer;
    QByteArray m_originalHash;
    QMap<qint64, QString> m_changedSegments; // Store changed segments
    QString m_currentText; // Store current text in the editor
    QString m_language;  // New member to store the language
};

#endif // DOCUMENT_H
