#include <QHBoxLayout>
#include <QTabWidget>
#include <QDebug>
#include "../codeeditor.h"
#include "movetonewview.h"
#include "../document.h"

MoveToNewView::MoveToNewView(QTabWidget *tabWidget, QWidget *parent)
    : QWidget(parent), m_tabWidget(tabWidget) {}

void MoveToNewView::execute() {
    // Get the index of the current tab
    int currentIndex = m_tabWidget->currentIndex();
    if (currentIndex < 0) {
        qDebug() << "No active tab found in the documentsTab.";
        return;
    }

    // Get the current tab widget
    QWidget* currentWidget = m_tabWidget->widget(currentIndex);
    if (!currentWidget) {
        qDebug() << "No widget found in the current tab.";
        return;
    }

    qDebug() << "Current widget type:" << currentWidget->metaObject()->className();

    // Locate the editor in the current widget (Document)
    QString currentContent;
    if (Document* currentDocument = qobject_cast<Document*>(currentWidget)) {
        // Assuming CodeEditor is a child of Document
        if (CodeEditor* editor = currentDocument->findChild<CodeEditor*>()) {
            currentContent = editor->toPlainText();
            qDebug() << "Content retrieved from CodeEditor:" << currentContent;
        } else {
            qDebug() << "The Document does not contain a CodeEditor.";
        }
    } else if (QTextEdit* editor = qobject_cast<QTextEdit*>(currentWidget)) {
        currentContent = editor->toPlainText();
        qDebug() << "Content retrieved directly from QTextEdit tab:" << currentContent;
    } else {
        qDebug() << "Current tab does not contain a known editor type.";
    }

    // Shift all tabs on the right of the current tab by one
    int tabCount = m_tabWidget->count();
    for (int i = tabCount - 1; i > currentIndex; --i) {
        QWidget* tab = m_tabWidget->widget(i);
        QString tabText = m_tabWidget->tabText(i);
        QIcon tabIcon = m_tabWidget->tabIcon(i);

        m_tabWidget->removeTab(i);
        m_tabWidget->insertTab(i + 1, tab, tabIcon, tabText);
    }

    // Create a new tab
    Document* newDocument = new Document("", m_tabWidget); // Assuming Document is your tab widget class
    if (!currentContent.isEmpty()) {
        if (CodeEditor* newEditor = newDocument->findChild<CodeEditor*>()) {
            newEditor->setPlainText(currentContent);
            qDebug() << "Content set for the new tab.";
        } else {
            qDebug() << "No editor found in the new Document to set content.";
        }
    } else {
        qDebug() << "No content to set for the new tab.";
    }

    // Insert the new tab
    m_tabWidget->insertTab(currentIndex + 1, newDocument, QIcon(), "New View");

    // Set focus to the new tab
    m_tabWidget->setCurrentIndex(currentIndex + 1);

    qDebug() << "Shifted tabs, created a new tab, and set its content at index:" << currentIndex + 1;
}
