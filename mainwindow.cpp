#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "document.h"
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QCommandLineParser>
#include <QCommandLineOption>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // Explicitly remove any existing tabs
    while (ui->documentsTab->count() > 0) {
        ui->documentsTab->removeTab(0);
    }

    Document *firstDoc = new Document("", this);
    ui->documentsTab->addTab(firstDoc, "Untitled Document");
    ui->documentsTab->setCurrentWidget(firstDoc);

    QStringList filePaths = QCoreApplication::arguments();
    for (const QString &filePath : filePaths) {
        if (filePath != QCoreApplication::applicationFilePath()) {
            openDocument(filePath);
        }
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::openDocument(const QString &filePath) {
    if (ui->documentsTab->count() > 0) {
        Document *existingDoc = qobject_cast<Document *>(ui->documentsTab->widget(0));
        if (existingDoc && existingDoc->filePath().isEmpty()) {
            existingDoc->openFile(filePath);
            ui->documentsTab->setTabText(0, QFileInfo(filePath).fileName());
            return;
        }
    }

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
        doc->saveFile();
    } else {
        QMessageBox::warning(this, tr("Error"), tr("No document to save."));
    }
}

void MainWindow::on_actionSave_As_triggered() {
    Document *doc = qobject_cast<Document *>(ui->documentsTab->currentWidget());
    if (doc) {
        QString filePath = QFileDialog::getSaveFileName(this, tr("Save File As"), "", tr("Text Files (*.txt);;All Files (*)"));
        if (!filePath.isEmpty()) {
            doc->saveFileAs(filePath);
        }
    } else {
        QMessageBox::warning(this, tr("Error"), tr("No document to save."));
    }
}

void MainWindow::on_documentsTab_tabCloseRequested(int index) {
    Document *doc = qobject_cast<Document *>(ui->documentsTab->widget(index));

    if (doc) {
        bool canClose = doc->closeDocument();

        if (canClose) {
            ui->documentsTab->removeTab(index);
        }
    }
}

void MainWindow::on_action_New_triggered() {
    Document *newDoc = new Document("", this);
    ui->documentsTab->addTab(newDoc, "Untitled Document");
    ui->documentsTab->setCurrentWidget(newDoc);
}

void MainWindow::on_action_Go_to_line_in_editor_triggered()
{
    Document *doc = qobject_cast<Document *>(ui->documentsTab->currentWidget());
    if (doc) {
        doc->goToLineNumberInEditor   ();
    } else {
        QMessageBox::warning(this, tr("Error"), tr("No document open."));
    }
}

void MainWindow::on_action_Go_to_line_in_text_triggered() {
    Document* doc = qobject_cast<Document*>(ui->documentsTab->currentWidget());
    if (doc) {
        doc->goToLineNumberInText(this);
    } else {
        QMessageBox::warning(this, tr("No Document"), tr("There is no document open."));
    }
}

void MainWindow::on_action_Close_triggered()
{
    int activeTabIndex = ui->documentsTab->currentIndex();  // Get the index of the active tab

    if (activeTabIndex != -1) {  // Check if there's an active tab
        on_documentsTab_tabCloseRequested(activeTabIndex);  // Reuse the existing close logic
    }
}

void MainWindow::on_actionC_lose_all_triggered() {
    closeAllDocuments();  // Call the method to close all documents
}

void MainWindow::closeAllDocuments() {
    int tabCount = ui->documentsTab->count();

    for (int i = tabCount - 1; i >= 0; --i) {
        Document* doc = qobject_cast<Document*>(ui->documentsTab->widget(i));
        if (doc) {
            bool canClose = doc->closeDocument();  // Trigger the unsaved changes check
            if (canClose) {
                ui->documentsTab->removeTab(i);
                delete doc;  // Clean up the document object
            } else {
                return;  // Stop closing further documents if the user cancels the close
            }
        }
    }
}

void MainWindow::on_actionC_3_triggered() {
    Document *doc = qobject_cast<Document *>(ui->documentsTab->currentWidget());
    if (doc) {
        qDebug() << "Menu action triggered for C++ syntax highlighting";
        doc->applySyntaxHighlighter("C++");
    } else {
        QMessageBox::warning(this, tr("Error"), tr("No document to format."));
    }
}

void MainWindow::on_actionPython_triggered()
{
    Document *doc = qobject_cast<Document *>(ui->documentsTab->currentWidget());
    if (doc) {
        qDebug() << "Menu action triggered for Python syntax highlighting";
        doc->applySyntaxHighlighter("Python");
    } else {
        QMessageBox::warning(this, tr("Error"), tr("No document to format."));
    }
}


void MainWindow::on_actionSav_e_all_triggered()
{
    // TODO: Save All
}


void MainWindow::on_actionCu_t_triggered()
{
    // TODO: Cut
}


void MainWindow::on_action_Copy_triggered()
{
    // TODO: Copy
}


void MainWindow::on_action_Paste_triggered()
{
    // TODO: Paste
}


void MainWindow::on_action_Undo_triggered()
{
    // TODO: Undo
}


void MainWindow::on_action_Redo_triggered()
{
    // TODO: Redo
}


void MainWindow::on_actionZoom_In_triggered()
{
    // TODO: Zoom In
}


void MainWindow::on_actionoom_Out_triggered()
{
    // TODO: Zoom Out
}


void MainWindow::on_action_Restore_Default_Zoom_triggered()
{
    // TODO: Restore Default Zoom
}


void MainWindow::on_action_Word_wrap_triggered()
{
    // TODO: Word Wrap
}

