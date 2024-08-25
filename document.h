#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QWidget>
#include <QFile>
#include <QMap>
#include <QCryptographicHash>
#include <QSyntaxHighlighter>
#include "languages/languagemanager.h"

class CodeEditor;

class Document : public QWidget {
    Q_OBJECT

public:
    Document(const QString &filePath, QWidget *parent = nullptr);
    QString filePath() const;
    void setFilePath(const QString &path);
    QString getLanguage() const;
    void openFile(const QString &filePath);
    void saveFile();
    void saveFileAs(const QString &newFilePath);
    bool closeDocument();
    void goToLineNumberInText(QWidget* parent);
    void goToLineNumberInEditor();
    void applySyntaxHighlighter(const QString &language);

signals:
    void saveError(const QString &error);
    void saveCompleted();

private:
    void loadContent();
    void updatePointers();
    void trackChanges();
    QByteArray calculateMD5Stream(QFile *file);
    QByteArray calculateModifiedMD5();

    QString m_filePath;
    QString m_fileExtension;
    QFile m_file;
    CodeEditor *editor;
    QSyntaxHighlighter *syntaxHighlighter;
    qint64 m_fileSize;
    qint64 m_startPointer;
    qint64 m_endPointer;
    QByteArray m_originalHash;
    QMap<qint64, QString> m_changedSegments;
    QString m_currentText;
    QString m_language;
};

#endif // DOCUMENT_H
