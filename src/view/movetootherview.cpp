#include <QHBoxLayout>
#include <QTabWidget>
#include <QDebug>
#include "movetootherview.h"
#include "../codeeditor.h"

void MoveToOtherView::execute() {
    qDebug() << "Executing MoveToOtherView";

    // Get the active tab in the documentsTab
    QWidget* currentTab = m_tabWidget->currentWidget();
    if (!currentTab) {
        qDebug() << "No active tab found in the documentsTab";
        return;
    }

    // Find the main CodeEditor widget (textEdit)
    CodeEditor* textEdit = currentTab->findChild<CodeEditor*>();
    if (!textEdit) {
        qDebug() << "No primary CodeEditor (textEdit) found in the current tab";
        return;
    }

    // Check if a second editor (textEdit2) already exists
    CodeEditor* textEdit2 = currentTab->findChild<CodeEditor*>("textEdit2");
    QLayout* layout = currentTab->layout();

    if (!layout) {
        qDebug() << "No layout found in current tab. Creating a new QHBoxLayout.";
        QHBoxLayout* newLayout = new QHBoxLayout(currentTab);
        currentTab->setLayout(newLayout);
        newLayout->addWidget(textEdit);
        newLayout->setStretchFactor(textEdit, 1);
        layout = newLayout;
    }

    QHBoxLayout* hLayout = qobject_cast<QHBoxLayout*>(layout);
    if (!hLayout) {
        qDebug() << "Existing layout is not QHBoxLayout. Wrapping it into QHBoxLayout.";
        QWidget* container = new QWidget(currentTab);
        container->setLayout(layout);

        hLayout = new QHBoxLayout(currentTab);
        currentTab->setLayout(hLayout);
        hLayout->addWidget(container);
        hLayout->setStretchFactor(container, 1);
    }

    if (textEdit2) {
        qDebug() << "Secondary editor (textEdit2) found. Removing it to restore single view.";

        // Remove the second editor safely
        if (hLayout->indexOf(textEdit2) != -1) {
            hLayout->removeWidget(textEdit2);
        }
        textEdit2->deleteLater();

        // Adjust layout for single view
        hLayout->setStretchFactor(textEdit, 1);

        currentTab->update();
        currentTab->setFocus();

        qDebug() << "Secondary editor removed and layout restored to single view.";
    } else {
        qDebug() << "No secondary editor found. Creating a second editor.";

        // Create and configure the second editor
        textEdit2 = new CodeEditor(currentTab);
        textEdit2->setObjectName("textEdit2");
        textEdit2->setPlainText(textEdit->toPlainText());

        // Add the second editor to the layout
        hLayout->addWidget(textEdit2);
        hLayout->setStretchFactor(textEdit, 1);
        hLayout->setStretchFactor(textEdit2, 1);

        currentTab->update();
        currentTab->setFocus();

        qDebug() << "Second editor created and added to the layout.";
    }
}
