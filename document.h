#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QWidget>
#include <QFile>
#include <QMap>
#include <QSyntaxHighlighter>
#include "languages/languagemanager.h"

class CodeEditor;

class Document : public QWidget {
    Q_OBJECT

public:
    Document(const QString &filePath, QWidget *parent = nullptr);
    ~Document();  // Destructor to clean up resources
    QString filePath() const;
    void setFilePath(const QString &path);
    QString getLanguage() const;
    void openFile(const QString &filePath);
    void saveFile();
    void saveFileAs(const QString &newFilePath);
    bool closeDocument();
    void goToLineNumberInText(QWidget* parent);
    void goToLineNumberInEditor();
    void applySyntaxHighlighter(const QString &language);

signals:
    void updateProgress(int value);
    void updateStatusMessage(const QString &message);

private:
    void loadContent();
    void trackChanges();
    bool promptForSave();
    void applyChangesToMemory();
    void moveCursorToFirstLine();
    bool checkForUnsavedChanges();
    void openFileAfterGuiIsReady();
    QMap<qint64, QString> m_originalSegments;
    bool compareText(const QString &text1, const QString &text2);

    QString m_filePath;
    QString m_fileExtension;
    QString m_originalText;
    QFile m_file;
    CodeEditor *editor;
    QSyntaxHighlighter *syntaxHighlighter;
    qint64 m_fileSize;
    QMap<qint64, QString> m_changedSegments;
    QString m_currentText;
    QString m_language;
    char* m_mmap;  // Memory-mapped file pointer
};

#endif // DOCUMENT_H
