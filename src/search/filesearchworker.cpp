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
                               m_options.matchCase ? QRegularExpression::NoPatternOption
                                                   : QRegularExpression::CaseInsensitiveOption);

    int lineNumber = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QRegularExpressionMatchIterator it = pattern.globalMatch(line);
        bool hasMatch = false;

        QString highlightedLine = line; // For highlighting
        qInfo() << "Highlight line from threadpool worker: " << highlightedLine;

        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();

            // Highlight the keywords in the line
            highlightedLine = highlightMatches(line, pattern);

            // Add the line number and the full line to `matches`
            result.matches.append(qMakePair(lineNumber, highlightedLine));

            result.matchCount++;
            hasMatch = true;
        }

        if (hasMatch) {
            // Append to `matchingLines` only if a match exists
            result.matchingLines.append(highlightedLine);
        }

        lineNumber++;
    }

    return result;
}

QString FileSearchWorker::highlightMatches(const QString& line, const QRegularExpression& pattern) {
    qInfo() << "highlightMatches input line: " << line;
    QString highlighted;
    int lastIndex = 0; // Track the last processed index in the line

    // Find all matches in the line
    QRegularExpressionMatchIterator it = pattern.globalMatch(line);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();

        // Append text from the last index to the current match start
        highlighted.append(line.mid(lastIndex, match.capturedStart(0) - lastIndex));

        // Wrap the matched text with <highlight> tags
        highlighted.append(QStringLiteral("<highlight>%1</highlight>").arg(match.captured(0)));

        // Update the last processed index
        lastIndex = match.capturedEnd(0);
    }

    // Append any remaining text after the last match
    highlighted.append(line.mid(lastIndex));
    qInfo() << "highlightMatches output line: " << line;

    return highlighted;
}

