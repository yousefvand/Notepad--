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
    qint64 chunkSize = fileSize / 100;  // Default chunk size is 1/100th of the file

    // For small files, read the entire file at once
    if (fileSize < 1 * 1024 * 1024) {  // Less than 1MB
        chunkSize = fileSize;
    }

    // Set a minimum chunk size of 4KB
    if (chunkSize < 4096) {
        chunkSize = 4096;  // 4KB minimum
    }

    // Optionally, set a maximum chunk size of 10MB
    if (chunkSize > 10 * 1024 * 1024) {  // Cap chunk size at 10MB
        chunkSize = 10 * 1024 * 1024;  // 10MB maximum
    }

    return chunkSize;
}

void FileLoaderWorker::startLoading() {
    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file:" << m_filePath;
        emit loadingError("File could not be opened.");
        return;
    }

    m_fileSize = file.size();
    if (m_fileSize <= 0) {
        emit loadingError("startLoading: File size is invalid!");
        return;
    }

    emit fileSizeDetermined(m_fileSize);

    // Calculate an appropriate chunk size
    qint64 chunkSize = calculateChunkSize(m_fileSize);

    // Use QTextStream for reading the file as text
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);

    qint64 bytesRead = 0;

    // Reading loop
    while (!in.atEnd()) {
        QString buffer = in.read(chunkSize);  // Read text data as UTF-8
        qDebug() << "Read chunk of size:" << buffer.size() << "Bytes read so far:" << bytesRead;

        if (buffer.isEmpty()) {
            qDebug() << "Read an empty buffer. Aborting...";
            break;
        }

        bytesRead += buffer.toUtf8().size();  // Update bytesRead based on the UTF-8 size
        qDebug() << "Total bytes read after update:" << bytesRead << "File size:" << m_fileSize;

        emit contentLoaded(buffer);

        // Update progress bar
        int progress = static_cast<int>((bytesRead * 100) / m_fileSize);
        qDebug() << "Progress updated to:" << progress << "%";
        emit loadingProgress(progress);
    }

    // Finalize the loading process
    if (bytesRead >= m_fileSize) {
        qDebug() << "Loading finished, total bytes read:" << bytesRead;
        emit loadingFinished();
    } else {
        qDebug() << "File not fully read. Bytes read:" << bytesRead;
    }
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
        QString chunk = in.read(chunkSize);  // Read text in chunks
        bytesRead += chunk.toUtf8().size();  // Count the number of bytes read (UTF-8)

        bufferedContent += chunk;

        // Emit content when the buffer reaches the chunk size
        if (bufferedContent.size() >= chunkSize) {
            emit contentLoaded(bufferedContent);
            bufferedContent.clear();
            QCoreApplication::processEvents();  // Keep UI responsive
        }

        // Emit progress
        int progress = static_cast<int>((bytesRead * 100) / fileSize);
        emit loadingProgress(progress);
    }

    // Emit remaining content in buffer
    if (!bufferedContent.isEmpty()) {
        emit contentLoaded(bufferedContent);
        bufferedContent.clear();
    }

    emit loadingFinished();  // Notify that the loading is complete
}
