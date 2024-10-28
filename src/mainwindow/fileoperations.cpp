#include "helpers.h"
#include <QFileInfo>
#include <QTabWidget>
#include <QMessageBox>
#include <QFileDialog>
#include <QMimeDatabase>
#include <QDebug>
#include "fileoperations.h"
#include "../ui_mainwindow.h"
#include "recentfiles.h"
#include "../mainwindow.h"
#include "../document.h"

FileOperations::FileOperations(MainWindow* mainWindow)
    : QObject(nullptr),  // Set QObject's parent to nullptr (optional)
    m_mainWindow(mainWindow) {}

bool FileOperations::closeDocument(Document* doc) {
    if (!doc) return false;  // Check if the document is valid

    // Logic to close the document
    doc->saveDocument();  // Example: Save the document before closing
    delete doc;   // Clean up the document
    return true;
}

void FileOperations::openDocumentTriggered() {
    // Show the file dialog to select a document
    QString filePath = QFileDialog::getOpenFileName(
        m_mainWindow, tr("Open File"), "", tr("Text Files (*.txt);;All Files (*)"));

    // If the user selects a valid file, open it
    if (!filePath.isEmpty()) {
        openDocument(filePath);
    }
}

void FileOperations::openDocument(const QString& filePath) {
    qDebug() << "Opening document: " << filePath;

    // Ensure m_mainWindow is valid
    if (!m_mainWindow) {
        qDebug() << "Error: MainWindow instance is null!";
        return;
    }

    // Ensure UI and documentsTab are valid
    auto ui = m_mainWindow->getUi();
    if (!ui || !ui->documentsTab) {
        qDebug() << "Error: UI or documentsTab is not initialized!";
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error: Unable to open file: " << filePath;
        QMessageBox::warning(m_mainWindow, tr("Error"), tr("Failed to open the file."));
        return;
    }

    if (ui->documentsTab->count() > 0) {
        Document* doc = qobject_cast<Document*>(ui->documentsTab->widget(0));

        if (doc && Helpers::isUntitledDocument(ui->documentsTab->tabText(0))) {
            QString content = doc->getEditorContent();  // Retrieve the editor content
            if (content.isEmpty()) {  // Check if the document is empty
                qDebug() << "Closing untitled and empty document.";
                ui->documentsTab->removeTab(0);
                doc->deleteLater();  // Schedule safe deletion
            } else {
                qDebug() << "Untitled document is not empty, skipping removal.";
            }
        }
    }

    if (!filePath.isEmpty()) { // TODO: Use in load session.
        RecentFiles::instance().addToRecentFiles(filePath);
        RecentFiles::instance().updateRecentFilesMenu(m_mainWindow->getUi()->actionRecent_Files);
    }

    qDebug() << filePath << " added to recent file list";

    // Create a new Document and add it to the tab widget
    Document* newDoc = new Document(filePath, static_cast<QWidget*>(m_mainWindow));
    ui->documentsTab->addTab(newDoc, QFileInfo(filePath).fileName());

    // Connect signals for the new document
    m_mainWindow->connectSignals(newDoc);

    // Set the new document as the current tab
    ui->documentsTab->setCurrentWidget(newDoc);
}

void FileOperations::newDocument() {
    // Create a new document with MainWindow as the parent
    Document* newDoc = new Document("", static_cast<QWidget*>(m_mainWindow));

    // Access the Ui object through the getter
    m_mainWindow->getUi()->documentsTab->addTab(newDoc, "Untitled Document");

    // Connect signals for the new document
    m_mainWindow->connectSignals(newDoc);

    // Set the new document as the current tab
    m_mainWindow->getUi()->documentsTab->setCurrentWidget(newDoc);
}

void FileOperations::openFolder(MainWindow* mainWindow) {

    QString path = QFileDialog::getExistingDirectory(
        mainWindow, QObject::tr("Select Folder"), QString(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (path.isEmpty()) {
        qDebug() << "No folder selected.";
        return;
    }

    // Get all files from the selected folder
    QDir directory(path);
    QStringList fileList = directory.entryList(QDir::Files);

    if (fileList.isEmpty()) {
        QMessageBox::information(mainWindow, QObject::tr("No Files Found"),
                                 QObject::tr("No files were found in the selected folder."));
        return;
    }

    QMimeDatabase mimeDatabase;
    bool textFilesFound = false;

    for (const QString& fileName : fileList) {
        QString filePath = directory.absoluteFilePath(fileName);

        QMimeType mimeType = mimeDatabase.mimeTypeForFile(filePath);

        if (mimeType.name().startsWith("text/")) {
            qDebug() << "Opening file with MIME type:" << mimeType.name();
            m_mainWindow->getFileOperations()->openDocument(filePath);  // Open in a new tab
            textFilesFound = true;
        } else {
            qDebug() << "Skipping file with MIME type:" << mimeType.name();
        }
    }

    if (!textFilesFound) {
        QMessageBox::information(mainWindow, QObject::tr("No Text Files Found"),
                                 QObject::tr("No text files were found in the selected folder based on MIME types."));
    }
}

void FileOperations::saveDocument() {
    Document *doc = qobject_cast<Document *>(m_mainWindow->getUi()->documentsTab->currentWidget());
    if (!doc) {
        QMessageBox::warning(m_mainWindow, tr("Error"), tr("No document to save."));
        return;
    }

    int currentIndex = m_mainWindow->getUi()->documentsTab->currentIndex();
    if (currentIndex == -1) return;

    QString title = m_mainWindow->getUi()->documentsTab->tabText(currentIndex);
    qDebug() << "Command save for Untitled Document, title is: " << title;

    if (Helpers::isUntitledDocument(title)) {
        qDebug() << "Save As dialog for Untitled Document Save command, title is: " << title;
        QString filePath = QFileDialog::getSaveFileName(
            m_mainWindow, tr("Save File As"), QString(), tr("Text Files (*.txt);;All Files (*)"));

        if (filePath.isEmpty()) return;
        doc->saveFileAs(filePath);
    } else {
        doc->saveFile();
    }
}

void FileOperations::saveDocumentAs() {
    // Get the current document from the tab widget
    Document* doc = qobject_cast<Document*>(m_mainWindow->getUi()->documentsTab->currentWidget());
    if (doc) {
        // Show the "Save As" dialog
        QString filePath = QFileDialog::getSaveFileName(
            m_mainWindow, QObject::tr("Save File As"), "",
            QObject::tr("Text Files (*.txt);;All Files (*)"));

        if (!filePath.isEmpty()) {
            // Save the file and update the tab name
            doc->saveFileAs(filePath);
            int tabIndex = m_mainWindow->getUi()->documentsTab->indexOf(doc);
            if (tabIndex != -1) {
                m_mainWindow->getUi()->documentsTab->setTabText(tabIndex, doc->fileName());
            }
        }
    } else {
        // Show a warning if no document is available
        QMessageBox::warning(m_mainWindow, QObject::tr("Error"), QObject::tr("No document to save."));
    }
}

void FileOperations::saveACopyAs() {
    // Get the current document from the tab widget
    Document* doc = qobject_cast<Document*>(m_mainWindow->getUi()->documentsTab->currentWidget());
    if (doc) {
        // Call the document's saveACopyAs method
        doc->saveAcopyAs();
    } else {
        // Show a warning if no document is available
        QMessageBox::warning(m_mainWindow, QObject::tr("Error"), QObject::tr("No document to save."));
    }
}

void FileOperations::saveAll() {
    QTabWidget* documentsTab = m_mainWindow->getUi()->documentsTab;
    int tabCount = documentsTab->count();
    qDebug() << "Save All triggered. Total open tabs:" << tabCount;

    // Iterate over all open tabs
    for (int i = 0; i < tabCount; ++i) {
        Document* doc = qobject_cast<Document*>(documentsTab->widget(i));
        if (doc && doc->isModified()) {
            qDebug() << "Saving modified document in tab:" << i;

            QString filePath = doc->filePath();

            // If the document doesn't have a file path, prompt the user to save it
            if (filePath.isEmpty()) {
                QString title = documentsTab->tabText(i);  // Get the current tab title

                QString savePath = QFileDialog::getSaveFileName(
                    m_mainWindow,
                    QObject::tr("Save %1").arg(title),  // Dialog title with current tab title
                    QString(),  // Default path
                    QObject::tr("Text Files (*.txt);;All Files (*)")
                    );

                if (savePath.isEmpty()) {
                    qDebug() << "User canceled save for tab:" << i;
                    continue;  // Skip saving this document
                }

                // Update the document's file path
                doc->setFilePath(savePath);

                // Directly set the tab title to the new file name
                documentsTab->setTabText(i, QFileInfo(savePath).fileName());
                qDebug() << "Updated tab title to:" << QFileInfo(savePath).fileName();
            }

            // Save the document
            doc->saveFile();
        }
    }

    qDebug() << "Save All completed.";
}

void FileOperations::closeActiveTab() {
    QTabWidget* documentsTab = m_mainWindow->getUi()->documentsTab;
    int currentIndex = documentsTab->currentIndex();

    if (currentIndex == -1) {
        qDebug() << "No active tab to close.";
        return;
    }

    Document* doc = qobject_cast<Document*>(documentsTab->widget(currentIndex));
    if (doc && closeDocument(doc)) {
        documentsTab->removeTab(currentIndex);  // Close the tab
        qDebug() << "Tab closed successfully.";
    } else {
        qDebug() << "Tab close operation canceled.";
    }
}

void FileOperations::closeAllDocuments() {
    QTabWidget* documentsTab = m_mainWindow->getUi()->documentsTab;  // Access the tab widget
    int tabCount = documentsTab->count();
    qDebug() << "Attempting to close all tabs. Current tab count:" << tabCount;

    QList<Document*> docsToClose;

    // Collect documents that can be closed
    for (int i = 0; i < tabCount; ++i) {
        Document* doc = qobject_cast<Document*>(documentsTab->widget(i));
        if (doc && doc->closeDocument()) {  // Check if the document can be closed
            docsToClose.append(doc);
        } else {
            qDebug() << "Skipped closing tab due to unsaved changes.";
        }
    }

    // Remove the collected documents and delete them
    for (Document* doc : docsToClose) {
        int index = documentsTab->indexOf(doc);
        if (index != -1) {
            documentsTab->removeTab(index);
            doc->deleteLater();  // Schedule safe deletion
        }
    }

    qDebug() << "All tabs have been closed.";
}

void FileOperations::closeAllButThisDocument() {
    QTabWidget* documentsTab = m_mainWindow->getUi()->documentsTab;  // Access the tab widget
    int activeTabIndex = documentsTab->currentIndex();  // Get the index of the active tab
    int tabCount = documentsTab->count();

    if (activeTabIndex == -1 || tabCount <= 1) {
        qDebug() << "No tabs to close or only one active tab.";
        return;
    }

    qDebug() << "Closing all tabs except the active tab at index:" << activeTabIndex;

    // Collect documents to close except the active one
    QList<Document*> docsToClose;
    for (int i = 0; i < tabCount; ++i) {
        if (i != activeTabIndex) {
            Document* doc = qobject_cast<Document*>(documentsTab->widget(i));
            if (doc && doc->closeDocument()) {  // Check if the document can be closed
                docsToClose.append(doc);
            } else {
                qDebug() << "Skipped closing tab due to unsaved changes.";
            }
        }
    }

    // Batch remove the collected tabs
    for (Document* doc : docsToClose) {
        int index = documentsTab->indexOf(doc);
        if (index != -1) {
            documentsTab->removeTab(index);
            doc->deleteLater();  // Safely delete the document
        }
    }

    qDebug() << "All tabs except the active one have been closed.";
}


