#include <QTabBar>
#include <QTimer>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QPointer>
#include <QFileDialog>
#include <QInputDialog>
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
#include "systemfind/systemfinddialog.h"
#include "systemreplace/systemreplacedialog.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    indentationManager(new IndentationManager(this)),
    findDialog(new FindDialog(this)),
    replaceDialog(new ReplaceDialog(this))
{

    ui->setupUi(this);  // Ensure the UI is set up before using it

    qDebug() << "Initializing MainWindow...";

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

    Helpers::RemoveMe(ui->documentsTab);
    Helpers::AddDefaultTab(ui->documentsTab);
    Helpers::zMenu(ui->menu_Language, this);

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

void MainWindow::on_actionFind_Next_triggered()
{
    if (!findDialog->getFind()) {
        qDebug() << "findDialog is null.";
        return;
    }

    findDialog->getFind()->findNext();
}

void MainWindow::on_actionFind_previoud_triggered()
{
    if (!findDialog->getFind()) {
        qDebug() << "findDialog is null.";
        return;
    }

    findDialog->getFind()->findPrevious();
}

void MainWindow::on_action_Replace_triggered()
{
    // Set the active document's editor in ReplaceDialog
    setActiveDocumentEditorInReplaceDialog();

    // Show the dialog without creating a new instance each time
    if (!replaceDialog->isVisible()) {
        replaceDialog->show();
    } else {
        replaceDialog->raise();  // Bring it to the front if it’s already open
        replaceDialog->activateWindow();
    }
}

void MainWindow::on_actionReplace_N_ext_triggered()
{
    if (!replaceDialog->getReplace()) {
        qDebug() << "replaceDialog is null.";
        return;
    }

    replaceDialog->getReplace()->replaceNext();
}

void MainWindow::on_actionReplace_P_revious_triggered()
{
    if (!replaceDialog->getReplace()) {
        qDebug() << "findDialog is null.";
        return;
    }

    replaceDialog->getReplace()->replacePrevious();
}

void MainWindow::on_actionFind_System_triggered() {
    qDebug() << "on_actionFind_System_triggered called";

    if (!m_systemFindDialog) {
        qDebug() << "Creating new SystemFindDialog";

        m_systemFindDialog = new SystemFindDialog(this);
        m_systemFindDialog->setWindowModality(Qt::NonModal);
        m_systemFindDialog->setAttribute(Qt::WA_DeleteOnClose);

        // Reset pointer when dialog is destroyed
        connect(m_systemFindDialog, &QObject::destroyed, this, [this]() {
            qDebug() << "SystemFindDialog destroyed. Resetting pointer.";
            m_systemFindDialog = nullptr;
        });

        // Ensure connections are established
        setupSearchResultDialogConnections();

        m_systemFindDialog->show();
    } else {
        qDebug() << "SystemFindDialog already exists. Bringing it to the front.";
        m_systemFindDialog->raise();
        m_systemFindDialog->activateWindow();
    }
}

void MainWindow::setupSearchResultDialogConnections() {
    QTimer::singleShot(0, this, [this]() {
        SystemSearchResultDialog* m_systemSearchResultDialog =
            m_systemFindDialog->findChild<SystemSearchResultDialog *>("SystemSearchResultDialog");

        if (m_systemSearchResultDialog) {
            qDebug() << "SystemSearchResultDialog found:" << m_systemSearchResultDialog;

            // Re-establish the signal-slot connection
            connect(m_systemSearchResultDialog, &SystemSearchResultDialog::openFileAtMatch,
                    this, &MainWindow::openSearchResult, Qt::UniqueConnection);

            qDebug() << "Signal-Slot Connection for SystemSearchResultDialog re-established.";
        } else {
            qDebug() << "SystemSearchResultDialog not found. Retrying...";
            QTimer::singleShot(100, this, &MainWindow::setupSearchResultDialogConnections);
        }
    });
}

void MainWindow::on_actionReplace_S_ystem_triggered()
{
    SystemReplaceDialog* systemReplaceDialog = new SystemReplaceDialog(this);
    systemReplaceDialog->setWindowModality(Qt::NonModal);
    systemReplaceDialog->show();
}

void MainWindow::on_actionGo_to_Line_in_Text_triggered()
{
    int lineNumber = QInputDialog::getInt(this, "Go to Line in Text", "Enter line number:");
    if (lineNumber < 1) {
        QMessageBox::warning(this, "Warning", "Enter a valid number.", QMessageBox::Ok);
    } else {
        getCurrentDocument()->editor()->goToLineInText(lineNumber);
    }
}

void MainWindow::on_actionGo_to_Line_in_Editor_triggered()
{
    int lineNumber = QInputDialog::getInt(this, "Go to Line in Editor", "Enter line number:");
    if (lineNumber < 1) {
        QMessageBox::warning(this, "Warning", "Enter a valid number.", QMessageBox::Ok);
    } else {
        getCurrentDocument()->editor()->gotoLineInEditor(lineNumber);
    }
}

void MainWindow::on_action_Find_triggered() {
    // Set the active document's editor in FindDialog
    setActiveDocumentEditorInFindDialog();

    // Show the dialog without creating a new instance each time
    if (!findDialog->isVisible()) {
        findDialog->show();
    } else {
        findDialog->raise();  // Bring it to the front if it’s already open
        findDialog->activateWindow();
    }
}







/* View Menu */











/* Helper Functions */





// FIXME: Wrong line selected
void MainWindow::openSearchResult(const QString &filePath, int lineNumber) {
    qInfo() << "openSearchResult called. File Path:" << filePath << ", Line Number:" << lineNumber;
    Document* doc = fileOperations->openDocument(filePath);
    if (!doc || !doc->editor()) {
        qFatal("Document or editor is null.");
        return;
    }

    // Use a weak pointer to ensure the document is still valid when the timer fires
    QPointer<Document> safeDoc(doc);

    QTimer::singleShot(200, this, [lineNumber, safeDoc]() {
        if (safeDoc && safeDoc->editor()) {
            safeDoc->editor()->goToLineInText(lineNumber);
        } else {
            qWarning() << "Document or editor is no longer available.";
        }
    });
}




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

// helper function
Document* MainWindow::getCurrentDocument() {
    int currentIndex = ui->documentsTab->currentIndex();
    if (currentIndex != -1) {
        // Assuming the document object is stored as a widget in the tab
        return qobject_cast<Document*>(ui->documentsTab->widget(currentIndex));
    }
    return nullptr;
}

// helper function
void MainWindow::setActiveDocumentEditorInFindDialog() {
    // Retrieve the active document from the current tab
    Document* activeDocument = qobject_cast<Document*>(ui->documentsTab->currentWidget());

    if (activeDocument) {
        // Get the editor from the active document
        CodeEditor* activeEditor = activeDocument->editor();  // Assumes Document::editor() returns a CodeEditor*

        if (activeEditor) {
            // Set the active editor in the find dialog
            findDialog->setEditor(activeEditor);
        } else {
            qWarning() << "Warning: No active editor found in document.";
        }
    } else {
        qWarning() << "Warning: No active document found.";
    }
}

// helper function
void MainWindow::setActiveDocumentEditorInReplaceDialog() {
    // Retrieve the active document from the current tab
    Document* activeDocument = qobject_cast<Document*>(ui->documentsTab->currentWidget());

    if (activeDocument) {
        // Get the editor from the active document
        CodeEditor* activeEditor = activeDocument->editor();  // Assumes Document::editor() returns a CodeEditor*

        if (activeEditor) {
            // Set the active editor in the find dialog
            replaceDialog->setEditor(activeEditor);
        } else {
            qWarning() << "Warning: No active editor found in document.";
        }
    } else {
        qWarning() << "Warning: No active document found.";
    }
}






