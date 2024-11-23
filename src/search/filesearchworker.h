#pragma once

#include <QObject>
#include <QRunnable>
#include <QRegularExpression>
#include <QMimeDatabase>
#include "searchoptions.h"

class FileSearchWorker : public QObject, public QRunnable {
    Q_OBJECT

public:
    FileSearchWorker(const QString& filePath, const SearchOptions& options);
    void run() override;

signals:
    void fileProcessed(const FileSearchResults& result);

private:
    FileSearchResults searchInFile();
    QString highlightMatches(const QString& line, const QRegularExpression& pattern);

    QString m_filePath;
    SearchOptions m_options;
};
