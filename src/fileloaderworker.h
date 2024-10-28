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
    qint64 m_fileSize;
    Document *document;
    QString m_filePath;
    QFile m_file;
};
