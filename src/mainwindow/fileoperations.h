#pragma once

#include <QObject>
#include <QString>
#include <QTabWidget>

class Document;
class MainWindow;

class FileOperations : public QObject {
    Q_OBJECT

public:
    explicit FileOperations(MainWindow* mainWindow, QObject* parent = nullptr);
    void closeTabByIndex(int index);
    void closeCurrentTab();

    static bool closeDocument(Document* doc);
    // This method moves the logic for opening a document from MainWindow
    Document* openDocument(const QString& filePath);
    void openDocumentTriggered();
    void newDocument();
    void openFolder(MainWindow* mainWindow);
    void saveDocument();
    void saveDocumentAs();
    void saveACopyAs();
    void saveAll();
    void closeAllDocuments();
    void closeAllButThisDocument();
    void copyFullPathToClipboard();
    void copyFileNameToClipboard();
    void copyDirectoryPathToClipboard();

private:
    MainWindow* m_mainWindow;
    QTabWidget* m_documentsTab;

    Document* getCurrentDocument() const;
};
