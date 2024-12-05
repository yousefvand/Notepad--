#include <QDebug>
#include "openinnewwindow.h"
#include "../mainwindow.h"
#include "../document.h"
#include "../codeeditor.h"

OpenInNewWindow::OpenInNewWindow(QTabWidget* tabWidget, QWidget* parent)
    : QWidget(parent), m_tabWidget(tabWidget) {}

void OpenInNewWindow::execute() {
    // Get the index of the current tab
    int currentIndex = m_tabWidget->currentIndex();
    if (currentIndex < 0) {
        qDebug() << "No active tab found in the documentsTab";
        return;
    }

    // Get the content of the current tab
    Document* currentDocument = qobject_cast<Document*>(m_tabWidget->widget(currentIndex));
    if (!currentDocument) {
        qDebug() << "Current tab is not a Document.";
        return;
    }

    CodeEditor* currentEditor = currentDocument->findChild<CodeEditor*>();
    QString currentContent;
    if (currentEditor) {
        currentContent = currentEditor->toPlainText();
    } else {
        qDebug() << "No CodeEditor found in the current Document.";
        return;
    }

    // Create a new MainWindow
    MainWindow* newWindow = new MainWindow();
    QTabWidget* newTabWidget = newWindow->findChild<QTabWidget*>("documentsTab");

    if (!newTabWidget) {
        qDebug() << "No QTabWidget found in the new MainWindow.";
        delete newWindow;
        return;
    }

    // Get the default tab created by the new window
    Document* untitledDocument = qobject_cast<Document*>(newTabWidget->widget(0));
    if (!untitledDocument) {
        qDebug() << "No default Document found in the new window.";
        delete newWindow;
        return;
    }

    // Set the content of the default tab
    CodeEditor* newEditor = untitledDocument->findChild<CodeEditor*>();
    if (newEditor) {
        newEditor->setPlainText(currentContent);
        qDebug() << "Content set for the default tab in the new window.";
    } else {
        qDebug() << "No CodeEditor found in the default Document to set content.";
    }

    // Rename the default tab to "New View"
    int defaultTabIndex = newTabWidget->indexOf(untitledDocument);
    if (defaultTabIndex >= 0) {
        newTabWidget->setTabText(defaultTabIndex, "New View");
    } else {
        qDebug() << "Could not find the default tab in the new window.";
    }

    // Show the new window
    newWindow->show();
    qDebug() << "New window created and displayed with the content copied.";
}
