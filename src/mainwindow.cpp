#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "document.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressBar>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QLabel>
#include <QTimer>
#include <QActionGroup>
#include <QMimeDatabase>
#include <QMimeType>
#include <QDir>

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

    if (arguments.size() <=1) {
        Document *newDoc = new Document("", this);  // Empty file path for untitled document
        ui->documentsTab->addTab(newDoc, "Untitled Document");
        ui->documentsTab->setCurrentWidget(newDoc);

        qDebug() << "Untitled Document created";
    } else {
        qDebug() << "Arguments passed:" << arguments;

        for (int i = 1; i < arguments.size(); ++i) {  // Skip argv(0), which is the app name
            QString filePath = arguments.at(i);
            qDebug() << "Opening document:" << filePath;
            openDocument(filePath);  // Open each file in a new tab
        }
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

    Document *newDoc = new Document(filePath, this);

    // Simulate loading the file (replace this with actual file loading logic)
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString fileContent = in.readAll();
        newDoc->openFile(fileContent);  // Load file content into the document
    }

    ui->documentsTab->addTab(newDoc, fileName);
    ui->documentsTab->setCurrentWidget(newDoc);

    qDebug() << "Document added with file path:" << filePath << " and file name:" << fileName;
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
    if (index < 0 || index >= ui->documentsTab->count()) {
        qDebug() << "Invalid index for closing tab:" << index;
        return;
    }

    Document *doc = qobject_cast<Document *>(ui->documentsTab->widget(index));
    if (doc) {
        // Check if the document has unsaved changes
        bool canClose = doc->closeDocument();  // Assuming closeDocument() checks for unsaved changes

        if (canClose) {
            // Disconnect signals before removing the tab to avoid cascading actions
            disconnect(ui->documentsTab, &QTabWidget::tabCloseRequested, this, &MainWindow::on_documentsTab_tabCloseRequested);

            // Now safely remove the tab
            ui->documentsTab->removeTab(index);

            // Delete the document after the tab is removed
            delete doc;

            qDebug() << "Closed tab at index:" << index << ". Remaining tabs:" << ui->documentsTab->count();

            // Reconnect the signal after tab removal to avoid duplicate triggering
            connect(ui->documentsTab, &QTabWidget::tabCloseRequested, this, &MainWindow::on_documentsTab_tabCloseRequested);
        } else {
            qDebug() << "User canceled closing tab at index:" << index;
        }
    }
}

void MainWindow::removeTabSafely(int index) {
    if (index < 0 || index >= ui->documentsTab->count()) {
        qDebug() << "Cannot remove tab, invalid index:" << index;
        return;
    }

    qDebug() << "Removing tab at index:" << index;

    // Safely remove the tab
    Document *doc = qobject_cast<Document *>(ui->documentsTab->widget(index));
    if (doc) {
        ui->documentsTab->removeTab(index);  // Remove the tab from the QTabWidget
        delete doc;  // Delete the document object
        qDebug() << "Tab removed and document deleted at index:" << index;
    } else {
        qDebug() << "Failed to remove tab at index:" << index;
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
    qDebug() << "Attempting to close all tabs. Current tab count:" << tabCount;

    for (int i = tabCount - 1; i >= 0; --i) {
        on_documentsTab_tabCloseRequested(i);
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

void MainWindow::on_actionOpen_Folder_triggered() {
    // Step 1: Open a folder selection dialog
    QString folderPath = QFileDialog::getExistingDirectory(this, tr("Select Folder"), QString(),
                                                           QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (folderPath.isEmpty()) {
        qDebug() << "No folder selected.";
        return;
    }

    // Step 2: Get all files from the selected folder
    QDir directory(folderPath);
    QStringList fileList = directory.entryList(QDir::Files);

    if (fileList.isEmpty()) {
        QMessageBox::information(this, tr("No Files Found"), tr("No files were found in the selected folder."));
        return;
    }

    // Step 3: Use QMimeDatabase to check MIME types and open text files
    QMimeDatabase mimeDatabase;
    bool textFilesFound = false;

    for (const QString &fileName : fileList) {
        QString filePath = directory.absoluteFilePath(fileName);

        // Get the MIME type of the file
        QMimeType mimeType = mimeDatabase.mimeTypeForFile(filePath);

        // Check if the MIME type is text/plain (or another text-related MIME type)
        if (mimeType.name().startsWith("text/")) {
            qDebug() << "Opening file with MIME type:" << mimeType.name();
            openDocument(filePath);  // This function opens the document in a new tab
            textFilesFound = true;
        } else {
            qDebug() << "Skipping file with MIME type:" << mimeType.name();
        }
    }

    if (!textFilesFound) {
        QMessageBox::information(this, tr("No Text Files Found"), tr("No text files were found in the selected folder based on MIME types."));
    }
}

void MainWindow::on_actionSa_ve_a_copy_as_triggered() {
    Document *doc = qobject_cast<Document *>(ui->documentsTab->currentWidget());
    doc->saveAcopyAs();
}

