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
    void saveError(const QString &error);
    void saveCompleted();
    void loadingStarted();
    void loadingProgress(int progress);
    void loadingFinished();

    void savingStarted();
    void savingProgress(int progress);
    void savingFinished();

public slots:
    void startFileLoadingAfterUIReady();

private:
    void loadContent();
    void loadContentAsync();
    void trackChanges();
    bool promptForSave();
    void loadEntireFile();
    bool checkForUnsavedChanges();
    bool compareText(const QString &text1, const QString &text2);
    void mirrorChangesToMemory(qint64 segmentStart, const QString &text);
    QByteArray calculateMD5Stream(QFile *file);
    QByteArray calculateModifiedMD5();

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
};

#endif // DOCUMENT_H