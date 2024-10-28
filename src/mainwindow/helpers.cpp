#include <QObject>
#include <QAction>
#include <QTabWidget>
#include <QPlainTextEdit>
#include <QMessageBox>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include "helpers.h"
#include "../document.h"
#include "../codeeditor.h"

bool Helpers::isUntitledDocument(const QString& title) {
    // Static QRegularExpression to avoid repeated creation
    static const QRegularExpression regex(
        R"((?:&?U&?n&?t&?i&?t&?l&?e&?d&?) (?:&?D&?o&?c&?u&?m&?e&?n&?t))"
        );

    return regex.match(title).hasMatch();
}

void Helpers::RemoveMe(QTabWidget* documentsTab) {
    if (documentsTab->count() > 0) {
        QWidget* firstTab = documentsTab->widget(0);
        documentsTab->removeTab(0);
        firstTab->deleteLater();  // Safe deletion
    }
}

void Helpers::AddDefaultTab(QTabWidget* documentsTab) {
    Document* defaultDoc = new Document("Untitled Document", documentsTab);
    documentsTab->addTab(defaultDoc, "Untitled Document");
    documentsTab->setCurrentWidget(defaultDoc);
}

void Helpers::CloseTab(QTabWidget* documentsTab, int index) {
    QWidget* tab = documentsTab->widget(index);
    if (tab) {
        documentsTab->removeTab(index);
        tab->deleteLater();  // Safe deletion
        qDebug() << "Tab at index" << index << "closed.";
    }
}

void Helpers::zMenu(QMenu* menuLanguage, QObject* receiver) {
    QMenu* menuZ = new QMenu("z", menuLanguage);
    QAction* actionZ80 = new QAction("Z80", menuZ);

    menuZ->addAction(actionZ80);
    menuLanguage->addMenu(menuZ);

    QObject::connect(actionZ80, &QAction::triggered, receiver, [receiver]() {
        QMetaObject::invokeMethod(receiver, "onActionZ80Triggered");
    });
}

bool Helpers::validateDocument(Document* doc, QWidget* parent) {
    if (!doc) {
        QMessageBox::warning(parent, "Print Error", "Failed to retrieve the document.");
        return false;
    }
    return true;
}

void Helpers::Print(QTabWidget* documentsTab, QWidget* parent) {
    // Create a printer object
    QPrinter printer;

    // Show a print dialog to the user
    QPrintDialog printDialog(&printer, parent);
    printDialog.setWindowTitle("Print Document");

    // Check if the user accepted the dialog
    if (printDialog.exec() != QDialog::Accepted) {
        QMessageBox::information(parent, "Print", "Printing was canceled.");
        return;
    }

    // Get the currently active document from the tab widget
    int currentTabIndex = documentsTab->currentIndex();
    if (currentTabIndex == -1) {
        QMessageBox::warning(parent, "Print Error", "No document to print.");
        return;
    }

    // Retrieve and validate the document
    Document* currentDoc = qobject_cast<Document*>(documentsTab->widget(currentTabIndex));
    if (!validateDocument(currentDoc, parent)) {
        return;
    }

    // Print the content of the editor associated with the document
    QTextDocument* textDoc = currentDoc->editor()->document();
    textDoc->print(&printer);

    QMessageBox::information(parent, "Print", "Document printed successfully.");
}



