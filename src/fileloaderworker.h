#pragma once

#include <QObject>
#include <QFile>
#include "document.h"

class Document;

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
    void loadingFinished();
    void savingFinished();
    void errorOccurred(const QString &error);
    void contentLoaded(const QString &content);

public slots:
    void startLoading();

private:
    Document *document;
    QString m_filePath;
    QFile m_file;
};
