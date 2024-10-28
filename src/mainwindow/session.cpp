#include <QFileDialog>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QTabBar>
#include "session.h"
#include "helpers.h"
#include "../mainwindow.h"
#include "../document.h"
#include "../codeeditor.h"
#include "recentfiles.h"
#include "../ui_mainwindow.h"

Session::Session(MainWindow* mainWindow, FileOperations* fileOperations)
    : m_mainWindow(mainWindow), m_fileOperations(fileOperations) {}

void Session::loadSession() {
    if (!m_fileOperations || !m_mainWindow) {
        qDebug() << "FileOperations or MainWindow is null!";
        return;
    }

    // Remove the empty "Untitled Document" if it exists
    removeUntitledDocumentIfEmpty();

    m_fileOperations->closeAllDocuments();  // Close all existing documents

    QString filePath = QFileDialog::getOpenFileName(
        m_mainWindow, tr("Load Session"), "", tr("JSON Files (*.Notepad--.json);;All Files (*)"));

    if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open session file!";
        return;
    }

    QByteArray sessionData = file.readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(sessionData);
    QJsonObject rootObj = jsonDoc.object();

    QJsonArray tabsArray = rootObj["tabs"].toArray();
    int activeTabIndex = rootObj["activeTab"].toInt();

    for (const QJsonValue& value : tabsArray) {
        QJsonObject tabData = value.toObject();
        loadTabFromSession(tabData);  // Load each tab from the session
    }

    // Focus on the active tab after all tabs are loaded
    if (m_mainWindow->getUi()->documentsTab->count() > activeTabIndex) {
        m_mainWindow->getUi()->documentsTab->setCurrentIndex(activeTabIndex);
    }

    // Final update to the recent files menu
    RecentFiles::instance().updateRecentFilesMenu(m_mainWindow->getUi()->actionRecent_Files);
}

void Session::removeUntitledDocumentIfEmpty() {
    QTabWidget* documentsTab = m_mainWindow->getUi()->documentsTab;

    for (int i = 0; i < documentsTab->count(); ++i) {
        QWidget* widget = documentsTab->widget(i);
        Document* doc = qobject_cast<Document*>(widget);

        // Check if it's an untitled document with no content
        if (doc && Helpers::isUntitledDocument(doc->filePath()) &&
            doc->editor()->toPlainText().isEmpty()) {
            qDebug() << "Removing empty Untitled Document.";
            documentsTab->removeTab(i);
            delete doc;  // Clean up the document
            break;  // Exit after removing the first matching document
        }
    }
}

void Session::loadTabFromSession(const QJsonObject &tabData) {
    QString filePath = tabData["filePath"].toString();
    bool isModified = tabData["isModified"].toBool();
    QString content = tabData["content"].toString();  // Read saved content
    int cursorPosition = tabData["cursorPosition"].toInt();  // Read cursor position

    qDebug() << "Restoring document from session with filePath:" << filePath;

    if (QFile::exists(filePath)) {
        // Load the file and add it to recent files
        Document* doc = new Document(filePath, m_mainWindow);
        m_mainWindow->getUi()->documentsTab->addTab(doc, QFileInfo(filePath).fileName());

        // Add to recent files and update the menu
        RecentFiles::instance().addToRecentFiles(filePath);
        RecentFiles::instance().updateRecentFilesMenu(m_mainWindow->getUi()->actionRecent_Files);

        m_mainWindow->connectSignals(doc);  // Connect signals
    } else if (isModified) {
        // Create a new tab with saved content if the file is missing but marked as modified
        Document* doc = new Document("Untitled", m_mainWindow);
        m_mainWindow->getUi()->documentsTab->addTab(doc, filePath);  // Use filePath as tab title
        doc->editor()->setPlainText(content);  // Restore the saved content

        // Set the tab title color to red to indicate a modified document
        int tabIndex = m_mainWindow->getUi()->documentsTab->indexOf(doc);
        if (tabIndex != -1) {
            m_mainWindow->getUi()->documentsTab->tabBar()->setTabTextColor(tabIndex, Qt::red);
        }

        // Set the cursor to the saved position
        QTextCursor cursor = doc->editor()->textCursor();
        cursor.setPosition(cursorPosition);
        doc->editor()->setTextCursor(cursor);

        m_mainWindow->connectSignals(doc);  // Connect signals
    } else {
        // Skip non-existent files that are not modified
        qDebug() << "Skipping tab for non-existent file: " << filePath;
    }
}

void Session::saveSession() {
    if (!m_mainWindow || !m_mainWindow->getUi() ||
        !m_mainWindow->getUi()->documentsTab) {
        qDebug() << "Failed to access documentsTab. MainWindow or UI not initialized.";
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(
        nullptr, tr("Save Session"), "", tr("JSON Files (*.Notepad--.json);;All Files (*)")
        );

    if (filePath.isEmpty()) return;

    QJsonArray tabsArray;

    for (int i = 0; i < m_mainWindow->getUi()->documentsTab->count(); ++i) {
        Document* doc = qobject_cast<Document*>(m_mainWindow->getUi()->documentsTab->widget(i));
        if (doc) {
            QJsonObject tabData;
            tabData["filePath"] = doc->filePath();
            tabData["isModified"] = doc->isModified();
            tabData["cursorPosition"] = doc->editor()->textCursor().position();
            tabData["content"] = doc->isModified() ? doc->editor()->toPlainText() : "";

            tabsArray.append(tabData);
        }
    }

    QJsonObject sessionData;
    sessionData["activeTab"] = m_mainWindow->getUi()->documentsTab->currentIndex();
    sessionData["tabs"] = tabsArray;

    QJsonDocument jsonDoc(sessionData);
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(jsonDoc.toJson(QJsonDocument::Indented));
        file.close();
        qDebug() << "Session saved to:" << filePath;
    } else {
        qDebug() << "Failed to open file for writing:" << filePath;
    }
}
