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

class CodeEditor;
class FileLoaderWorker;
class LanguageManager;

class Document : public QWidget {
    Q_OBJECT

public:
    explicit Document(const QString &filePath, QWidget *parent = nullptr);
    ~Document();

    void saveDocument();
    bool promptForSave();
    QString filePath() const;
    QString fileName() const;
    void setFilePath(const QString &path);
    QString getLanguage() const;
    void openFile(const QString &filePath);
    void saveFile();
    void onWorkerTaskCompleted();
    bool checkForUnsavedChanges();
    void saveFileAs(const QString &newFilePath);
    void saveAcopyAs();
    bool closeDocument();
    void goToLineNumberInText(QWidget* parent);
    void goToLineNumberInEditor();
    void applySyntaxHighlighter(const QString &language);
    QString getEditorContent() const;
    bool compareText(const QString &text1, const QString &text2);
    bool isModified() const;
    void setModified(bool modified);
    void startLoading();
    void finishLoading();
    CodeEditor* editor() const { return m_editor; }
    FileLoaderWorker* worker() const { return m_fileLoaderWorker; }
    bool isLoading() const;
    QString originalFileContent;
    int savedCursorPosition() const;
    void setSavedCursorPosition(int position);
    QThread* workerThread() const;

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
    void onSavingStarted();
    void handleProgressBarHiding();

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
    QString m_fileName;
    QString m_fileExtension;
    QFile m_file;
    CodeEditor *m_editor;
    std::unique_ptr<QSyntaxHighlighter> syntaxHighlighter;
    qint64 m_fileSize;
    QMap<qint64, QString> m_changedSegments;
    QString m_currentText;
    QString m_language;
    int m_lastProgress = 0;
    int m_lastSmoothedProgress = 0;
    int m_smoothProgressUpdateInterval = 1;
    qint64 m_totalBytesInserted = 0;
    bool m_isModified;
    int m_savedCursorPosition = 0;
};
