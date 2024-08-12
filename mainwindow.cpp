#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "codeeditor.h"
#include "document.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    if (ui->documentsTab->count() > 0) {
        ui->documentsTab->removeTab(0); // Remove the first tab (index 0)
    }

    // Create the first Document tab with a code editor
    Document *firstDoc = new Document("", this); // Pass an empty string for the file path
    ui->documentsTab->addTab(firstDoc, "Untitled Document");
    ui->documentsTab->setCurrentWidget(firstDoc); // Set the first tab as current
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_action_Open_triggered() {
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Text Files (*.txt);;All Files (*)"));
    if (!filePath.isEmpty()) {
        Document *currentDoc = qobject_cast<Document *>(ui->documentsTab->currentWidget());

        if (currentDoc && currentDoc->content().isEmpty()) {
            // If the current document is empty, load the file content into it
            currentDoc->openFile(filePath);
            ui->documentsTab->setTabText(ui->documentsTab->currentIndex(), QFileInfo(filePath).fileName());
        } else {
            // If the current document is not empty, create a new tab for the file
            Document *newDoc = new Document(filePath, this);
            ui->documentsTab->addTab(newDoc, QFileInfo(filePath).fileName());
            ui->documentsTab->setCurrentWidget(newDoc);
        }
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
