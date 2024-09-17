#include "fileloaderworker.h"
#include "document.h"
#include <QApplication>
#include <QMetaObject>
#include <QThread>
#include <QDebug>
#include <QFile>

FileLoaderWorker::FileLoaderWorker(const QString &filePath, Document *doc, QObject *parent)
    : QObject(parent), m_filePath(filePath), m_file(filePath), document(doc) {
    qDebug() << "FileLoaderWorker initialized for file:" << filePath;
    qDebug() << "FileLoaderWorker thread at initialization:" << QThread::currentThread();
}

FileLoaderWorker::~FileLoaderWorker() {
    if (m_file.isOpen()) {
        m_file.close();
        qDebug() << "File closed in destructor";
    }
}

qint64 calculateChunkSize(qint64 fileSize) {
    qint64 chunkSize = 0;

    if (fileSize < 1 * 1024 * 1024) {  // Files less than 1MB
        chunkSize = fileSize;  // Load the entire file in one go
    } else if (fileSize < 100 * 1024 * 1024) {  // Files between 1MB and 100MB
        chunkSize = fileSize / 100;  // Divide the file into 100 chunks
    } else {  // Files larger than 100MB
        chunkSize = 1 * 1024 * 1024;  // Use 1MB chunks for large files
    }

    return chunkSize;
}

void FileLoaderWorker::startLoading() {
    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit errorOccurred("Failed to open file: " + m_filePath);
        return;
    }

    qint64 totalBytesRead = 0;
    qint64 fileSize = file.size();
    qint64 chunkSize = 1024 * 1024;  // Define your chunk size

    while (!file.atEnd()) {
        QByteArray chunk = file.read(chunkSize);

        if (chunk.isEmpty()) {
            break;  // Stop if nothing more to read
        }

        emit contentLoaded(QString::fromUtf8(chunk));  // Emit content for each chunk
        totalBytesRead += chunk.size();  // Track actual size of each chunk

        int progress = (totalBytesRead * 100) / fileSize;  // Calculate progress
        emit loadingProgress(progress);
    }

    emit loadingFinished();  // Signal that loading is finished
}

void FileLoaderWorker::saveFile(const QString &filePath, const QString &fileContent) {
    qDebug() << "Saving file to path: " << filePath;
    QFile file(filePath);

    qDebug() << "Attempting to save file to: " << filePath;

    // Try opening the file for writing
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file for writing: " << file.errorString();
        emit errorOccurred("Failed to open file for saving.");
        return;
    }

    QTextStream out(&file);
    out << fileContent;

    // Check for QTextStream status to ensure it wrote correctly
    if (out.status() != QTextStream::Ok) {
        qDebug() << "Failed to write file content to: " << filePath;
        emit errorOccurred("Failed to write to file.");
    } else {
        out.flush();
        file.flush();
        file.close();
        qDebug() << "File saved successfully to: " << filePath;
        emit savingFinished();
    }
}

void FileLoaderWorker::loadFile(const QString &filePath) {
    qDebug() << "Opening file from path: " << filePath;
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly)) {
        emit errorOccurred(QString("Error opening file: %1").arg(filePath));
        return;
    }

    qint64 totalBytesRead = 0;
    const int chunkSize = 1024 * 1024;  // Read in 1MB chunks
    QByteArray chunk;

    while (!file.atEnd()) {
        chunk = file.read(chunkSize);
        if (chunk.isEmpty()) {
            break;
        }

        totalBytesRead += chunk.size();
        emit contentLoaded(QString::fromUtf8(chunk));

        // Emit progress every 1MB read
        if (totalBytesRead % (1024 * 1024) == 0) {
            emit loadingProgress(totalBytesRead);
        }
    }

    // Emit progress one last time to mark the completion
    emit loadingProgress(totalBytesRead);
    emit loadingFinished();
}
