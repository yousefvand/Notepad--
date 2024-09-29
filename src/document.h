#pragma once

#include <QWidget>
#include <QThread>
#include <QFile>
#include <QString>
#include <QMap>
#include <QLabel>
#include <QProgressBar>
#include <QSyntaxHighlighter>
#include "fileloaderworker.h"
#include "languages/languagemanager.h"

class CodeEditor;
class FileLoaderWorker;

class Document : public QWidget {
    Q_OBJECT

public:
    Document(const QString &filePath, QWidget *parent = nullptr);
    ~Document();

    void saveDocument();
    bool promptForSave();
    QString filePath() const;
    void setFilePath(const QString &path);
    QString getLanguage() const;
    void openFile(const QString &filePath);
    void saveFile();
    void onWorkerTaskCompleted();
    bool checkForUnsavedChanges();
    void saveFileAs(const QString &newFilePath);
    bool closeDocument();
    void goToLineNumberInText(QWidget* parent);
    void goToLineNumberInEditor();
    void applySyntaxHighlighter(const QString &language);

signals:
    void uiReady();
    void saveError(const QString &error);
    void saveCompleted();
    void loadingStarted();
    void loadingProgress(int progress);
    void loadingFinished();
    void hideProgressBar();
    void savingStarted();
    void savingFinished();
    void savingProgress(int progress);

public slots:
    void onLoadingStarted();
    void onLoadingProgress(int progress);
    void onLoadingFinished();
    void onLoadingError(const QString &error);
    void insertContentIntoEditor(const QString &content);
    void onSavingStarted();

private slots:
    void onContentLoaded(const QString &chunk);
    void onFileSizeDetermined(qint64 fileSize);
    void onSavingProgress(int progress);
    void onSavingFinished();

private:
    void loadContent();
    void loadContentAsync();
    void trackChanges();
    void loadEntireFile();
    bool compareText(const QString &text1, const QString &text2);
    void mirrorChangesToMemory(qint64 segmentStart, const QString &text);
    QString calculateMD5Stream(QFile *file);
    QString calculateModifiedMD5();

    bool m_isLoading;
    QString m_bufferedContent;
    bool m_isSaving = false;
    qint64 m_totalBytesRead;
    QThread *m_workerThread;
    FileLoaderWorker *m_fileLoaderWorker;
    QLabel *m_statusLabel;
    QProgressBar *m_progressBar;

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
    int m_lastProgress;
    int m_lastSmoothedProgress = 0;
    int m_smoothProgressUpdateInterval = 1;
};

