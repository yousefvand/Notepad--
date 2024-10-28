#pragma once

#include <QObject>
#include <QString>

class Document;
class MainWindow;

class FileOperations : public QObject {
    Q_OBJECT

public:
    explicit FileOperations(MainWindow* mainWindow);

    static bool closeDocument(Document* doc);
    // This method moves the logic for opening a document from MainWindow
    void openDocument(const QString& filePath);
    void openDocumentTriggered();
    void newDocument();
    void openFolder(MainWindow* mainWindow);
    void saveDocument();
    void saveDocumentAs();
    void saveACopyAs();
    void saveAll();
    void closeActiveTab();
    void closeAllDocuments();
    void closeAllButThisDocument();

private:
    MainWindow* m_mainWindow;
};
