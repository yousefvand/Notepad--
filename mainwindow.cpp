#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "document.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QCommandLineParser>
#include <QCommandLineOption>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // Remove the initial empty tab if it exists
    if (ui->documentsTab->count() > 0) {
        ui->documentsTab->removeTab(0); // Remove the first tab
    }

    // Create the initial "Untitled Document" tab
    Document *firstDoc = new Document("", this); // Pass an empty string for the file path
    ui->documentsTab->addTab(firstDoc, "Untitled Document");
    ui->documentsTab->setCurrentWidget(firstDoc); // Set the first tab as current

    // Open files passed as command line arguments
    QStringList filePaths = QCoreApplication::arguments();
    for (const QString &filePath : filePaths) {
        if (filePath != QCoreApplication::applicationFilePath()) { // Skip the application path
            openDocument(filePath);
        }
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::openDocument(const QString &filePath) {
    // Check if there is an empty tab to reuse
    if (ui->documentsTab->count() > 0) {
        Document *existingDoc = qobject_cast<Document *>(ui->documentsTab->widget(0));
        if (existingDoc && existingDoc->filePath().isEmpty()) {
            // Reuse the existing empty tab
            existingDoc->openFile(filePath); // Open the file in the existing tab
            ui->documentsTab->setTabText(0, QFileInfo(filePath).fileName()); // Update the tab title
            return; // Exit as we've reused the tab
        }
    }

    // If no empty tab exists, create a new document tab
    Document *newDoc = new Document(filePath, this);
    ui->documentsTab->addTab(newDoc, QFileInfo(filePath).fileName());
    ui->documentsTab->setCurrentWidget(newDoc);
}

void MainWindow::on_action_Open_triggered() {
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Text Files (*.txt);;All Files (*)"));
    if (!filePath.isEmpty()) {
        openDocument(filePath);
    }
}

void MainWindow::on_action_Save_triggered() {
    Document *doc = qobject_cast<Document *>(ui->documentsTab->currentWidget());
    if (doc) {
        doc->saveFile(); // Call saveFile method of Document
    } else {
        QMessageBox::warning(this, tr("Error"), tr("No document to save."));
    }
}

void MainWindow::on_actionSave_As_triggered() {
    Document *doc = qobject_cast<Document *>(ui->documentsTab->currentWidget());
    if (doc) {
        QString filePath = QFileDialog::getSaveFileName(this, tr("Save File As"), "", tr("Text Files (*.txt);;All Files (*)"));
        if (!filePath.isEmpty()) {
            doc->saveFileAs(filePath); // Call saveFileAs method of Document
        }
    } else {
        QMessageBox::warning(this, tr("Error"), tr("No document to save."));
    }
}

void MainWindow::on_documentsTab_tabCloseRequested(int index) {
    // Get the current document from the tab
    Document *doc = qobject_cast<Document *>(ui->documentsTab->widget(index));

    if (doc) {
        // Call the closeDocument method to handle the closing logic
        bool canClose = doc->closeDocument(); // This method should return true if it's okay to close

        if (canClose) {
            // If the document has been closed, remove the tab
            ui->documentsTab->removeTab(index);
        }
    }
}

void MainWindow::on_action_New_triggered() {
    Document *newDoc = new Document("", this); // Pass an empty string for the file path
    ui->documentsTab->addTab(newDoc, "Untitled Document");
    ui->documentsTab->setCurrentWidget(newDoc);
}

void MainWindow::on_actionC_3_triggered() {
    Document *doc = qobject_cast<Document *>(ui->documentsTab->currentWidget());
    if (doc) {
        doc->applySyntaxHighlighter(); // Now this is public
    } else {
        QMessageBox::warning(this, tr("Error"), tr("No document to format."));
    }
}
