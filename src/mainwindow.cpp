#include <QTabBar>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QFileDialog>
#include <QPlainTextEdit>
#include "mainwindow.h"
#include "codeeditor.h"
#include "settings.h"
#include "mainwindow/textoperations.h"
#include "mainwindow/recentfiles.h"
#include "mainwindow/session.h"
#include "src/ui_mainwindow.h"
#include "helpers.h"
#include "indentation/indentationdialog.h"
#include "find/finddialog.h"
#include "replace/replacedialog.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    indentationManager(new IndentationManager(this)) {

    qDebug() << "Initializing MainWindow...";

    ui->setupUi(this);  // Ensure the UI is set up before using it

    Helpers::RemoveMe(ui->documentsTab);
    Helpers::AddDefaultTab(ui->documentsTab);
    Helpers::zMenu(ui->menu_Language, this);

    fileOperations = new FileOperations(this);
    textOperations = new TextOperations(ui->documentsTab);
    indentationManager = new IndentationManager(this, this);
    indentationManager->setupIndentationMenu(ui->action_Custom, ui->action_Default_Setting, ui->smartIndent);
    indentationManager->loadSmartIndentSetting();

    ui->actionRecent_Files->setVisible(false);
    RecentFiles::instance().loadRecentFiles();
    RecentFiles::instance().setFileOperations(fileOperations);
    RecentFiles::instance().updateRecentFilesMenu(ui->actionRecent_Files);

    connect(ui->documentsTab, &QTabWidget::tabCloseRequested, this,
            [this](int index) { fileOperations->closeTabByIndex(index); });
    connect(ui->actionBegin_End_Select, &QAction::toggled, this,
            [this](bool checked) { textOperations->handleCheckboxToggle(checked); });

    // Initialize Formatting after setting up the UI
    formatting = new Formatting(this, ui->documentsTab);
    formatting->setupActions(ui->actionWindows_Format, ui->action_Unix_OS_X_Format, ui->action_Old_Mac_Format);

    qDebug() << "MainWindow initialized...";
}

MainWindow::~MainWindow() {
    qDebug() << "MainWindow destructor called.";
    if (settings) {
        settings = nullptr;  // Just null the pointer since it's a singleton
    }
    delete ui;
    delete fileOperations;
    delete textOperations;
}

Ui::MainWindow* MainWindow::getUi() const {
    return ui;
}

/* File Menu */

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

void MainWindow::on_action_Save_triggered()
{
    fileOperations->saveDocument();
}

void MainWindow::on_actionSave_As_triggered()
{
    fileOperations->saveDocumentAs();
}

void MainWindow::on_actionSa_ve_a_copy_as_triggered()
{
    fileOperations->saveACopyAs();
}

void MainWindow::on_actionSav_e_all_triggered()
{
    fileOperations->saveAll();
}

void MainWindow::on_action_Close_triggered()
{
    fileOperations->closeCurrentTab();
}

void MainWindow::on_actionC_lose_all_triggered()
{
    fileOperations->closeAllDocuments();
}

void MainWindow::on_actionClose_all_BUT_current_document_triggered()
{
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

void MainWindow::on_action_Print_triggered()
{
    helpers.Print(ui->documentsTab, this);
}

void MainWindow::on_actionE_xit_triggered()
{
    this->close();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    qDebug() << "Application is closing. Saving settings...";
    RecentFiles::instance().saveRecentFiles();
    qApp->quit();
    event->accept();
}

/* Edit Menu */

void MainWindow::on_action_Undo_triggered()
{
    textOperations->undo();
}

void MainWindow::on_action_Redo_triggered()
{
    textOperations->redo();
}

void MainWindow::on_actionCu_t_triggered()
{
    textOperations->cut();
}

void MainWindow::on_action_Copy_triggered()
{
    textOperations->copy();
}

void MainWindow::on_action_Paste_triggered()
{
    textOperations->paste();
}

void MainWindow::on_action_Delete_triggered()
{
    textOperations->deleteText();
}

void MainWindow::on_actionSelect_All_triggered()
{
    textOperations->selectAll();
}

// Begin/End Select implemented somewhere else.

void MainWindow::on_actionCopy_Full_Path_to_Clipboard_triggered()
{
    fileOperations->copyFullPathToClipboard();
}

void MainWindow::on_actionCopy_File_Name_to_Clipboard_triggered()
{
    fileOperations->copyFileNameToClipboard();
}

void MainWindow::on_actionCopy_Directory_to_Clipboard_triggered()
{
    fileOperations->copyDirectoryPathToClipboard();
}

// helper function
FileOperations* MainWindow::getFileOperations() const
{
    return fileOperations;
}

void MainWindow::on_action_UPPERCASE_triggered()
{
    textOperations->convertToUpperCase();
}

void MainWindow::on_action_lowercase_triggered()
{
    textOperations->convertToLowerCase();
}

void MainWindow::on_actionDuplicate_Current_Line_triggered()
{
    textOperations->duplicateCurrentLine();
}

void MainWindow::on_actionDelete_Current_Line_triggered()
{
    textOperations->deleteCurrentLine();
}

void MainWindow::on_action_Move_Line_Up_triggered()
{
    textOperations->moveLineUp();
}

void MainWindow::on_actionMove_Line_Down_triggered()
{
    textOperations->moveLineDown();
}

void MainWindow::on_action_Trim_Trailing_Space_triggered()
{
    formatting->trimTrailingSpace();
}

void MainWindow::on_actionTrim_Leading_Space_triggered()
{
    formatting->trimLeadingSpace();
}

void MainWindow::on_actionTrim_Leading_and_Trailing_Space_triggered()
{
    formatting->trimLeadingAndTrailingSpace();
}

void MainWindow::on_action_EOL_to_Space_triggered()
{
    formatting->eolToSpace();
}

void MainWindow::on_actionTab_to_Space_triggered()
{
    formatting->tabToSpace(2); // TODO: Number of spaces is hardcoded.
}

void MainWindow::on_action_Space_to_TAB_all_triggered()
{
    Helpers::notImplemented(this);
    // formatting->spaceToTab(2); // FIXME: Various methods doesn't work
}

void MainWindow::on_actionS_pace_to_Tab_Leading_triggered()
{
    Helpers::notImplemented(this);
    // formatting->spaceToTabLeading(2); // FIXME: Various methods doesn't work
}

void MainWindow::on_action_Default_Setting_triggered() {
    // Code to handle Default Setting selection
    indentationManager->setToDefaultIndentation();
    saveIndentationSetting("Default");
    qDebug() << "Default indentation setting applied.";
}

void MainWindow::on_action_Custom_triggered()
{
    if (indentationManager) indentationManager->openIndentationDialog();
}

void MainWindow::on_smartIndent_triggered()
{
    Helpers::notImplemented(this);
    bool isChecked = isSmartIndentChecked();
    indentationManager->saveSmartIndentSetting(isChecked);
}

// helper function
void MainWindow::setSmartIndentChecked(bool checked) {
    ui->smartIndent->setChecked(checked);
}

// helper function
bool MainWindow::isSmartIndentChecked() const {
    return ui->smartIndent->isChecked();
}

/* Search Menu */

void MainWindow::on_action_Find_triggered() {
    FindDialog::showDialog(this);
}

void MainWindow::on_actionFind_Next_triggered()
{
    qDebug() << "Implement on_actionFind_Next_triggered";
    // TODO: Implement
}

void MainWindow::on_actionFind_previoud_triggered()
{
    qDebug() << "Implement on_actionFind_previoud_triggered";
    // TODO: Implement
}

void MainWindow::on_action_Replace_triggered()
{
    ReplaceDialog::showDialog(this);
}

void MainWindow::on_actionReplace_N_ext_triggered()
{
    qDebug() << "Implement on_actionReplace_N_ext_triggered";
    // TODO: Implement
}

void MainWindow::on_actionReplace_P_revious_triggered()
{
    qDebug() << "Implement on_actionReplace_P_revious_triggered";
    // TODO: Implement
}

void MainWindow::on_actionFind_System_triggered()
{
    qDebug() << "Implement on_actionFind_System_triggered";
    // TODO: Implement
}

void MainWindow::on_actionReplace_S_ystem_triggered()
{
    qDebug() << "Implement on_actionReplace_S_ystem_triggered";
    // TODO: Implement
}

void MainWindow::on_actionGo_to_Line_in_Text_triggered()
{
    Helpers::gotoLineInText(this, getCurrentDocument()->editor());
}

void MainWindow::on_actionGo_to_Line_in_Editor_triggered()
{
    Helpers::gotoLineInEditor(this, getCurrentDocument()->editor());
}

/* View Menu */






















void MainWindow::onActionZ80Triggered()
{
    qDebug() << "Z80 action triggered!";
}




// helper function
void MainWindow::connectSignals(Document *doc)
{
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

// helper function
void MainWindow::applyColorCoding(Document* doc, bool isModified)
{
    int index = ui->documentsTab->indexOf(doc);
    if (index == -1) return;  // Invalid tab index

    QColor tabColor = isModified ? Qt::red : QColor(0, 100, 0);  // Dark Green for saved

    ui->documentsTab->tabBar()->setTabTextColor(index, tabColor);
    qDebug() << "Applied color" << tabColor.name() << "to tab index:" << index;
}

void MainWindow::saveIndentationSetting(const QString& setting) {
    auto* settings = Settings::instance();  // Use the singleton Settings instance
    settings->setValue("IndentationSettings/indentationSetting", setting);
    qDebug() << "Saved indentation setting:" << setting;
}


Document* MainWindow::getCurrentDocument() {
    int currentIndex = ui->documentsTab->currentIndex(); // Assuming you use a QTabWidget named documentsTab
    if (currentIndex != -1) {
        // Assuming the document object is stored as a widget in the tab
        return qobject_cast<Document*>(ui->documentsTab->widget(currentIndex));
    }
    return nullptr;
}






















