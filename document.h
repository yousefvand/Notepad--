#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QWidget>
#include <QString>
#include "CodeEditor.h"

class Document : public QWidget {
    Q_OBJECT

public:
    explicit Document(QWidget *parent = nullptr);
    Document(const QString &filePath, QWidget *parent = nullptr);
    void openFile(const QString &filePath);
    void saveFile();
    void saveFileAs(const QString &filePath);

private:
    CodeEditor *editor; // Pointer to the CodeEditor
    QString filePath; // Public member to store the file path
};

#endif // DOCUMENT_H
