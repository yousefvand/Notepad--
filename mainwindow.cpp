#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "codeeditor.h"
#include "document.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QCommandLineParser>
#include <QCommandLineOption>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // Remove any placeholder or default tab if it exists
    if (ui->documentsTab->count() > 0) {
        ui->documentsTab->removeTab(0); // Remove the first tab (index 0)
    }

    // Get command-line arguments
    QStringList filePaths = QCoreApplication::arguments();

    if (filePaths.size() > 1) { // If there are more arguments than just the application path
        for (int i = 1; i < filePaths.size(); ++i) {
            openDocument(filePaths.at(i));
        }
    }

    if (ui->documentsTab->count() == 0) {
        // If no documents were opened via command line, create a new empty tab
        Document *firstDoc = new Document("", this); // Pass an empty string for the file path
        ui->documentsTab->addTab(firstDoc, "Untitled Document");
        ui->documentsTab->setCurrentWidget(firstDoc); // Set the first tab as current
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::openDocument(const QString &filePath) {
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
    // Handle the tab close request
    QWidget *widget = ui->documentsTab->widget(index);
    ui->documentsTab->removeTab(index);
    delete widget; // Free the memory
}
