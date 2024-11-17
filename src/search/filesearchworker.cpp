#include <QFile>
#include <QTextStream>
#include <QMimeDatabase>
#include "filesearchworker.h"

FileSearchWorker::FileSearchWorker(const QString& filePath, const SearchOptions& options)
    : m_filePath(filePath), m_options(options) {
    setAutoDelete(true);
}

void FileSearchWorker::run() {
    QMimeDatabase mimeDb;
    QMimeType mimeType = mimeDb.mimeTypeForFile(m_filePath);
    if (mimeType.name().startsWith("text/")) {
        FileSearchResults result = searchInFile();
        if (result.matchCount > 0) {
            emit fileProcessed(result);
        }
    }
}

FileSearchResults FileSearchWorker::searchInFile() {
    FileSearchResults result;
    result.filePath = m_filePath;
    result.matchCount = 0;

    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return result;

    QTextStream in(&file);
    QRegularExpression pattern(m_options.keyword,
                               m_options.matchCase ? QRegularExpression::NoPatternOption : QRegularExpression::CaseInsensitiveOption);

    int lineNumber = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QRegularExpressionMatchIterator it = pattern.globalMatch(line);
        bool hasMatch = false;

        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            result.matchingLines.append(highlightMatches(line, pattern));
            result.matchCount++;
            hasMatch = true;
        }

        if (!hasMatch) lineNumber++;
    }
    return result;
}

QString FileSearchWorker::highlightMatches(const QString& line, const QRegularExpression& pattern) {
    QString highlighted = line;
    return highlighted.replace(pattern, "<highlight>" + m_options.keyword + "</highlight>");
}
