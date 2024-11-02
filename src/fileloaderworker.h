#pragma once

#include <QObject>
#include <QFile>
#include "document.h"

class Document;
class MainWindow;

class FileLoaderWorker : public QObject {
    Q_OBJECT

public:
    explicit FileLoaderWorker(const QString &filePath, Document *doc, QObject *parent = nullptr);
    ~FileLoaderWorker();
    void saveFile(const QString &filePath, const QString &fileContent);
    void loadFile(const QString &filePath);

signals:
    void loadingStarted();
    void loadingProgress(qint64 bytesRead);
        void savingProgress(qint64 progress);
    void loadingFinished();
    void savingStarted();
    void savingFinished();
    void errorOccurred(const QString &error);
    void contentLoaded(const QString &chunk);
    void loadingError(const QString &errorMsg);
    void fileSizeDetermined(qint64 fileSize);

public slots:
    void startLoading();

private:
    QFile m_file;
    QString m_filePath;
    Document *document;
    qint64 m_fileSize;
};
