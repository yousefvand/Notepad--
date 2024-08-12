#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "CodeEditor.h"
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
    Document *firstDoc = new Document(this);
    ui->documentsTab->addTab(firstDoc, "Untitled Document");
    ui->documentsTab->setCurrentWidget(firstDoc); // Set the first tab as current
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_action_Open_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Text Files (*.txt);;All Files (*)"));
    if (!fileName.isEmpty()) {
        Document *doc = new Document(fileName, this); // Create Document with the file path
        ui->documentsTab->addTab(doc, QFileInfo(fileName).fileName()); // Add the Document to the tab widget
        ui->documentsTab->setCurrentWidget(doc); // Switch to the new document tab
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
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File As"), "", tr("Text Files (*.txt);;All Files (*)"));
        if (!fileName.isEmpty()) {
            doc->saveFileAs(fileName); // Call saveFileAs method of Document
        }
    } else {
        QMessageBox::warning(this, tr("Error"), tr("No document to save."));
    }
}
