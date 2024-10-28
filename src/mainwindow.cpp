#include <QTabBar>
#include <QFileDialog>
#include <QPlainTextEdit>
#include "mainwindow.h"
#include "codeeditor.h"
#include "mainwindow/recentfiles.h"
#include "mainwindow/session.h"
#include "ui_mainwindow.h"
#include "mainwindow/helpers.h"

#include <QActionGroup>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    fileOperations(new FileOperations(this)),
    session(new Session(this, fileOperations))
{
    qDebug() << "Initializing MainWindow...";
    ui->setupUi(this);

    Helpers::RemoveMe(ui->documentsTab);
    Helpers::AddDefaultTab(ui->documentsTab);
    Helpers::zMenu(ui->menu_Language, this);

    Formatting* eol = new Formatting(this);
    eol->setupActions(ui->actionWindows_Format, ui->action_Unix_OS_X_Format, ui->action_Old_Mac_Format);

    connect(ui->documentsTab, &QTabWidget::tabCloseRequested, this,
            [this](int index) { Helpers::CloseTab(ui->documentsTab, index); });

    fileOperations = new FileOperations(this);

    ui->actionRecent_Files->setVisible(false);
    RecentFiles::instance().loadRecentFiles();
    RecentFiles::instance().setFileOperations(fileOperations);
    RecentFiles::instance().updateRecentFilesMenu(ui->actionRecent_Files);
    qDebug() << "MainWindow initialized.";
}

MainWindow::~MainWindow() {
    delete ui;
    delete session;
    delete fileOperations;
}

Ui::MainWindow* MainWindow::getUi() const {
    return ui;
}

void MainWindow::on_action_New_triggered()
{
    fileOperations->newDocument();
}

void MainWindow::on_action_Open_triggered()
{
    fileOperations->openDocumentTriggered();
}

void MainWindow::on_actionOpen_Folder_triggered()
{
    fileOperations->openFolder(this);
}

void MainWindow::on_action_Save_triggered() {
    fileOperations->saveDocument();
}

void MainWindow::on_actionSave_As_triggered() {
    fileOperations->saveDocumentAs();
}

void MainWindow::on_actionSa_ve_a_copy_as_triggered() {
    fileOperations->saveACopyAs();
}

void MainWindow::on_actionSav_e_all_triggered() {
    fileOperations->saveAll();
}

void MainWindow::on_action_Close_triggered() {
    fileOperations->closeActiveTab();
}

void MainWindow::on_actionC_lose_all_triggered() {
    fileOperations->closeAllDocuments();
}

void MainWindow::on_actionClose_all_BUT_current_document_triggered() {
    fileOperations->closeAllButThisDocument();
}

void MainWindow::on_actionLoad_session_triggered()
{
    session->loadSession();
}

void MainWindow::on_actionSave_session_triggered()
{
    session->saveSession();
}

void MainWindow::on_action_Print_triggered() {
    helpers.Print(ui->documentsTab, this);
}

void MainWindow::on_actionE_xit_triggered()
{
    this->close();
}


























FileOperations* MainWindow::getFileOperations() const {
    return fileOperations;
}

void MainWindow::onActionZ80Triggered()
{
    qDebug() << "Z80 action triggered!";
}

void MainWindow::closeEvent(QCloseEvent* event) {
    qDebug() << "Application is closing. Saving settings...";
    RecentFiles::instance().saveRecentFiles();
    qApp->quit();
    event->accept();
}





























void MainWindow::connectSignals(Document *doc) {
    if (!doc) return;

    // Connect modification change to apply color coding.
    connect(doc->editor(), &QPlainTextEdit::modificationChanged, this, [this, doc](bool changed) {
        applyColorCoding(doc, changed);
    });

    // Connect the worker’s save completion signal.
    connect(doc->worker(), &FileLoaderWorker::savingFinished, this, [this, doc]() {
        applyColorCoding(doc, false);
    });

    // Connect undo and redo actions to the editor.
    connect(ui->action_Undo, &QAction::triggered, doc->editor(), &QPlainTextEdit::undo);
    connect(ui->action_Redo, &QAction::triggered, doc->editor(), &QPlainTextEdit::redo);

    // Enable/disable the undo and redo actions based on editor state.
    connect(doc->editor(), &QPlainTextEdit::undoAvailable, ui->action_Undo, &QAction::setEnabled);
    connect(doc->editor(), &QPlainTextEdit::redoAvailable, ui->action_Redo, &QAction::setEnabled);

    // Ensure undo/redo actions are enabled/disabled initially.
    ui->action_Undo->setEnabled(doc->editor()->document()->isUndoAvailable());
    ui->action_Redo->setEnabled(doc->editor()->document()->isRedoAvailable());
}




void MainWindow::applyColorCoding(Document* doc, bool isModified) {
    int index = ui->documentsTab->indexOf(doc);
    if (index == -1) return;  // Invalid tab index

    QColor tabColor = isModified ? Qt::red : QColor(0, 100, 0);  // Dark Green for saved

    ui->documentsTab->tabBar()->setTabTextColor(index, tabColor);
    qDebug() << "Applied color" << tabColor.name() << "to tab index:" << index;
}






















