#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "document.h"
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QProgressBar>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include "mainwindow.h"
#include <QLabel>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QProgressBar>
#include <QTimer>
#include <QActionGroup>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    qDebug() << "Tab widget pointer: " << ui->documentsTab;
    ui->documentsTab->setTabsClosable(true);

    QMenu *menuLanguage = ui->menu_Language;
    QMenu *menuZ = new QMenu("z", this);
    QAction *actionZ80 = new QAction("Z80", this);
    menuZ->addAction(actionZ80);
    menuLanguage->addMenu(menuZ);
    connect(actionZ80, &QAction::triggered, this, &MainWindow::onActionZ80Triggered);

    QActionGroup *group = new QActionGroup(this);
    group->setExclusive(true);  // Ensure only one action is checked at a time

    // Add actions to the group
    group->addAction(ui->actionWindows_Format);
    group->addAction(ui->action_Unix_OS_X_Format);
    group->addAction(ui->action_Old_Mac_Format);

    // Optionally, set a default action checked
    ui->action_Unix_OS_X_Format->setChecked(true);

    qDebug() << "Setting up UI and clearing existing tabs.";

    // Explicitly remove any existing tabs
    while (ui->documentsTab->count() > 0) {
        ui->documentsTab->removeTab(0);
    }

    ui->documentsTab->setTabsClosable(true);

    connect(ui->documentsTab, &QTabWidget::tabCloseRequested, this, &MainWindow::on_documentsTab_tabCloseRequested);

    qDebug() << "Tabs cleared, preparing to open files.";

    // Get arguments from argv and open files
    QStringList arguments = QCoreApplication::arguments();

    // Debug the list of arguments
    qDebug() << "Arguments passed:" << arguments;

    for (int i = 1; i < arguments.size(); ++i) {  // Skip argv(0), which is the app name
        QString filePath = arguments.at(i);
        qDebug() << "Opening document:" << filePath;
        openDocument(filePath);  // Open each file in a new tab
    }

    qDebug() << "Initialization complete, emitting uiReady signal.";
    initialize();  // Initialize UI readiness
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::initialize() {
    QTimer::singleShot(100, this, [this]() {
        qDebug() << "Emitting uiReady signal after 100ms delay";
        qDebug() << "About to emit uiReady signal...";
        emit uiReady();
    });
}

void MainWindow::openDocument(const QString &filePath) {
    if (!QFile::exists(filePath)) {
        qDebug() << "Error: File does not exist: " << filePath;
        return;
    }

    QString fileName = QFileInfo(filePath).fileName();
    qDebug() << "Opening file:" << filePath;

    // Create a new Document instance
    Document *newDoc = new Document(filePath, this);

    // Check if the document was created
    if (!newDoc) {
        qDebug() << "Failed to create Document.";
        return;
    }

    // Check if the tab widget exists before adding the tab
    if (!ui->documentsTab) {
        qDebug() << "Tab widget is nullptr!";
        return;
    }

    // Add the new document to the tab widget
    int tabIndex = ui->documentsTab->addTab(newDoc, fileName);
    ui->documentsTab->setCurrentWidget(newDoc);

    qDebug() << "Document added at index: " << tabIndex << " with file name: " << fileName;
    qDebug() << "Tab count after adding document: " << ui->documentsTab->count();
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
    qDebug() << "Attempting to close tab at index: " << index;

    // Retrieve the widget at the given index
    QWidget *widget = ui->documentsTab->widget(index);

    if (!widget) {
        qDebug() << "No widget found at index " << index;
        return;
    }

    // Cast the widget to a Document
    Document *doc = qobject_cast<Document *>(widget);

    if (!doc) {
        qDebug() << "Failed to cast widget to Document at index " << index;
        return;
    }

    // Get the current content of the editor
    QString editorContent = doc->getEditorContent();
    qDebug() << "Editor content length: " << editorContent.length();

    // Compare current editor content with the original file content
    if (!doc->compareText(editorContent, doc->originalFileContent)) {
        // If the content is different, prompt the user to save, discard, or cancel
        qDebug() << "Unsaved changes detected in the document.";
        if (doc->promptForSave()) {
            // User decided to save or discard changes
            qDebug() << "User decided to save or discard. Closing tab.";
            ui->documentsTab->removeTab(index);  // Remove the tab only after handling unsaved changes
            delete doc;  // Clean up the document object
        } else {
            // User canceled the close operation
            qDebug() << "User canceled closing the tab.";
            return;
        }
    } else {
        // No unsaved changes, close the tab directly
        qDebug() << "No unsaved changes. Closing tab.";
        ui->documentsTab->removeTab(index);
        delete doc;  // Clean up the document object
    }

    qDebug() << "Tab closed. Remaining tabs: " << ui->documentsTab->count();
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
    qDebug() << "Attempting to close all tabs. Current tab count:" << tabCount;

    for (int i = tabCount - 1; i >= 0; --i) {
        Document* doc = qobject_cast<Document*>(ui->documentsTab->widget(i));
        if (doc) {
            bool canClose = doc->closeDocument();
            if (canClose) {
                ui->documentsTab->removeTab(i);
                delete doc;
                qDebug() << "Tab at index" << i << "closed.";
            } else {
                qDebug() << "User canceled closing tab at index" << i;
                return;  // Stop closing if the user cancels
            }
        }
    }

    qDebug() << "Remaining tabs after close all:" << ui->documentsTab->count();
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

void MainWindow::onActionZ80Triggered()
{
    qDebug() << "Z80 action triggered!";
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QApplication::aboutQt();
}


void MainWindow::on_action_About_Notepad_triggered()
{
    // TODO: About Remisa Yousefvand
}

