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
    qint64 chunkSize = fileSize / 100;

    if (fileSize > 500 * 1024 * 1024) {  // If file is larger than 500MB
        chunkSize = 8 * 1024 * 1024;  // Set chunk size to 8MB
    } else if (fileSize < 1 * 1024 * 1024) {
        chunkSize = fileSize;  // Read small files in one go
    }

    if (chunkSize < 4096) chunkSize = 4096;  // Minimum 4KB
    if (chunkSize > 10 * 1024 * 1024) chunkSize = 10 * 1024 * 1024;  // Maximum 10MB

    return chunkSize;
}

void FileLoaderWorker::startLoading() {
    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit loadingError("File could not be opened.");
        return;
    }

    m_fileSize = file.size();
    emit fileSizeDetermined(m_fileSize);
    emit loadingStarted();  // Ensure UI knows loading has started

    const qint64 chunkSize = 16 * 1024;  // Use 16KB chunks for finer updates
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);

    qint64 bytesRead = 0;
    int lastReportedProgress = 0;
    auto lastUpdateTime = std::chrono::steady_clock::now();

    while (!in.atEnd()) {
        QString buffer = in.read(chunkSize);
        if (buffer.isEmpty()) break;

        bytesRead += buffer.toUtf8().size();
        emit contentLoaded(buffer);  // Emit content incrementally

        // Calculate current progress percentage
        int currentProgress = static_cast<int>((bytesRead * 100) / m_fileSize);

        // Throttle updates: emit only if 100ms has passed since the last update
        auto now = std::chrono::steady_clock::now();
        if (currentProgress > lastReportedProgress &&
            std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdateTime).count() > 100) {

            lastReportedProgress = currentProgress;
            emit loadingProgress(currentProgress);  // Emit progress

            // Update last update time and process UI events
            lastUpdateTime = now;
            QCoreApplication::processEvents();
        }
    }

    emit loadingProgress(100);  // Ensure progress reaches 100%
    emit loadingFinished();
    file.close();
}

void FileLoaderWorker::saveFile(const QString &filePath, const QString &fileContent) {
    qDebug() << "Saving file to path: " << filePath;
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file for writing: " << file.errorString();
        emit errorOccurred("Failed to open file for saving.");
        return;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);  // Set encoding
    qint64 totalBytes = fileContent.toUtf8().size();
    qint64 bytesWritten = 0;
    qint64 chunkSize = calculateChunkSize(totalBytes);  // Reuse the same chunk size logic

    while (bytesWritten < totalBytes) {
        QString chunk = fileContent.mid(bytesWritten, chunkSize);  // Write in chunks
        out << chunk;
        out.flush();  // Ensure the data is written

        bytesWritten += chunk.toUtf8().size();  // Increment bytes written

        // Emit saving progress
        int progress = static_cast<int>((bytesWritten * 100) / totalBytes);
        emit savingProgress(progress);

        // Allow the UI to remain responsive
        QCoreApplication::processEvents();
    }

    // Finalize the save operation
    if (out.status() == QTextStream::Ok) {
        file.close();
        emit savingFinished();
        qDebug() << "File saved successfully to: " << filePath;
    } else {
        emit errorOccurred("Failed to write to file.");
    }
}

void FileLoaderWorker::loadFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file:" << filePath;
        emit loadingError("File could not be opened.");
        return;
    }

    qint64 fileSize = file.size();
    qint64 chunkSize = calculateChunkSize(fileSize);  // Calculate optimal chunk size for the file

    if (chunkSize < 4096) {
        chunkSize = 4096;  // Ensure a minimum chunk size
    }

    qint64 bytesRead = 0;
    QString bufferedContent;
    QTextStream in(&file);

    while (!in.atEnd()) {
        QString buffer = in.read(chunkSize);  // Read the file chunk
        if (buffer.isEmpty()) {
            qDebug() << "Read an empty buffer. Stopping at bytesRead: " << bytesRead;
            break;
        }

        bytesRead += buffer.toUtf8().size();  // Update the total bytes read
        qDebug() << "Read chunk of size: " << buffer.size() << " Total bytesRead: " << bytesRead;

        emit contentLoaded(buffer);
        QCoreApplication::processEvents();

        // Update progress bar
        int progress = static_cast<int>((bytesRead * 100) / m_fileSize);
        qDebug() << "Updated progress: " << progress << "%";
        emit loadingProgress(progress);
    }

    // Emit remaining content in buffer
    if (!bufferedContent.isEmpty()) {
        emit contentLoaded(bufferedContent);
        bufferedContent.clear();
    }

    emit loadingFinished();  // Notify that the loading is complete
}
