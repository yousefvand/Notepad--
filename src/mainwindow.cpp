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
#include "mainwindow/mainwindowconfigloader.h"
#include "mainwindow/textoperations.h"
#include "mainwindow/recentfiles.h"
#include "mainwindow/session.h"
#include "src/ui_mainwindow.h"
#include "helpers.h"
#include "find/finddialog.h"
#include "replace/replacedialog.h"
#include "systemfind/systemfinddialog.h"
#include "systemreplace/systemreplacedialog.h"
#include "view/movetootherview.h"
#include "view/movetonewview.h"
#include "view/openinnewwindow.h"
#include "view/wordwrap.h"
#include "aboutdialog.h"
// encodings
#include "encoding/interpret_as_utf_8.h"
#include "encoding/interpret_as_utf_8_without_bom.h"
#include "encoding/interpret_as_utf_16_be.h"
#include "encoding/interpret_as_utf_16_le.h"
#include "encoding/interpret_as_dialog.h"
#include "encoding/interpret_as_utf_8.h"
#include "encoding/interpret_as_utf_7.h"
#include "encoding/interpret_as_utf_32_le.h"
#include "encoding/interpret_as_utf_32_be.h"
#include "encoding/interpret_as_utf_16.h"
#include "encoding/interpret_as_us_ascii.h"
#include "encoding/interpret_as_tscii.h"
#include "encoding/interpret_as_tis_620.h"
#include "encoding/interpret_as_shift_jts.h"
#include "encoding/interpret_as_scsu.h"
#include "encoding/interpret_as_koi8_u.h"
#include "encoding/interpret_as_koi8_r.h"
#include "encoding/interpret_as_iso_8859_9.h"
#include "encoding/interpret_as_iso_8859_8.h"
#include "encoding/interpret_as_iso_8859_7.h"
#include "encoding/interpret_as_iso_8859_6.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    indentationManager(new IndentationManager(this)),
    findDialog(new FindDialog(this)),
    replaceDialog(new ReplaceDialog(this)),
    m_currentTabIndex(-1),
    m_formerTabIndex(-1)
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
    fileOperations->newDocument();
    Helpers::zMenu(ui->menu_Language, this);
    m_wordWrap = new WordWrap(this);

    m_mainWindowConfigLoader = new MainWindowConfigLoader(this);
    m_mainWindowConfigLoader->loadMainWindowConfig();

    connect(ui->documentsTab, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
    if (ui->documentsTab->count() > 0) {
        m_currentTabIndex = ui->documentsTab->currentIndex();
    }

    connect(ui->action_Full_Screen, &QAction::toggled, this, &MainWindow::on_action_Full_Screen_toggled);

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
    delete m_systemFindDialog;
    delete m_systemReplaceDialog;
    delete m_mainWindowConfigLoader;
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
        setupSearchResultDialogConnectionsForFind();

        m_systemFindDialog->show();
    } else {
        qDebug() << "SystemFindDialog already exists. Bringing it to the front.";
        m_systemFindDialog->raise();
        m_systemFindDialog->activateWindow();
    }
}

void MainWindow::on_actionReplace_S_ystem_triggered()
{
    if (!m_systemReplaceDialog) {
        qDebug() << "Creating new SystemReplaceDialog";

        m_systemReplaceDialog = new SystemReplaceDialog(this);
        m_systemReplaceDialog->setWindowModality(Qt::NonModal);
        m_systemReplaceDialog->setAttribute(Qt::WA_DeleteOnClose);

        // Reset pointer when dialog is destroyed
        connect(m_systemReplaceDialog, &QObject::destroyed, this, [this]() {
            qDebug() << "SystemReplaceDialog destroyed. Resetting pointer.";
            m_systemReplaceDialog = nullptr;
        });

        // Ensure connections are established
        setupSearchResultDialogConnectionsForReplace();

        m_systemReplaceDialog->show();
    } else {
        qDebug() << "SystemReplaceDialog already exists. Bringing it to the front.";
        m_systemReplaceDialog->raise();
        m_systemReplaceDialog->activateWindow();
    }
}

void MainWindow::setupSearchResultDialogConnectionsForFind() {
    QTimer::singleShot(0, this, [this]() {
        SystemSearchResultDialog* m_systemSearchResultDialog =
            m_systemFindDialog->findChild<SystemSearchResultDialog *>("SystemSearchResultDialog");

        if (m_systemSearchResultDialog) {
            qDebug() << "SystemSearchResultDialog found:" << m_systemSearchResultDialog;

            // Re-establish the signal-slot connection
            connect(m_systemSearchResultDialog, &SystemSearchResultDialog::openFileAtMatch,
                    this, &MainWindow::openSearchResult);

            qDebug() << "Signal-Slot Connection for SystemSearchResultDialog re-established.";
        } else {
            qDebug() << "SystemSearchResultDialog not found. Retrying...";
            QTimer::singleShot(100, this, &MainWindow::setupSearchResultDialogConnectionsForFind);
        }
    });
}

void MainWindow::setupSearchResultDialogConnectionsForReplace() {
    QTimer::singleShot(0, this, [this]() {
        SystemSearchResultDialog* m_systemSearchResultDialog =
            m_systemReplaceDialog->findChild<SystemSearchResultDialog *>("SystemSearchResultDialog");

        if (m_systemSearchResultDialog) {
            qDebug() << "SystemSearchResultDialog found:" << m_systemSearchResultDialog;

            // Re-establish the signal-slot connection
            connect(m_systemSearchResultDialog, &SystemSearchResultDialog::openFileAtMatch,
                    this, &MainWindow::openSearchResult);

            qDebug() << "Signal-Slot Connection for SystemSearchResultDialog re-established.";
        } else {
            qDebug() << "SystemSearchResultDialog not found. Retrying...";
            QTimer::singleShot(100, this, &MainWindow::setupSearchResultDialogConnectionsForReplace);
        }
    });
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

void MainWindow::on_action_Show_Tabs_triggered(bool checked)
{
    Settings::instance()->saveSetting("View", "ShowTabs", checked);

    for (int i = 0; i < ui->documentsTab->count(); ++i) {
        Document *doc = qobject_cast<Document *>(ui->documentsTab->widget(i));
        if (doc) {
            doc->editor()->setShowTabs(checked);
        }
    }
}

void MainWindow::on_actionShow_Spaces_triggered(bool checked)
{
    Settings::instance()->saveSetting("View", "ShowSpaces", checked);

    for (int i = 0; i < ui->documentsTab->count(); ++i) {
        Document *doc = qobject_cast<Document *>(ui->documentsTab->widget(i));
        if (doc) {
            doc->editor()->setShowSpaces(checked);
        }
    }
}

void MainWindow::on_actionShow_End_of_Lines_triggered(bool checked)
{
    Settings::instance()->saveSetting("View", "ShowEOL", checked);

    for (int i = 0; i < ui->documentsTab->count(); ++i) {
        Document *doc = qobject_cast<Document *>(ui->documentsTab->widget(i));
        if (doc) {
            doc->editor()->setShowEOL(checked);
        }
    }
}

void MainWindow::on_actionShow_All_Characters_triggered(bool checked)
{
    Settings::instance()->saveSetting("View", "ShowAllCharacters", checked);

    for (int i = 0; i < ui->documentsTab->count(); ++i) {
        Document *doc = qobject_cast<Document *>(ui->documentsTab->widget(i));
        if (doc) {
            doc->editor()->setShowAllCharacters(checked);
        }
    }
}

void MainWindow::on_actionShow_Indent_Guide_triggered(bool checked)
{
    Settings::instance()->saveSetting("View", "ShowIndentGuide", checked);

    for (int i = 0; i < ui->documentsTab->count(); ++i) {
        Document *doc = qobject_cast<Document *>(ui->documentsTab->widget(i));
        if (doc) {
            doc->editor()->setShowIndentGuide(checked);
        }
    }
}

void MainWindow::on_actionShow_Wrap_Symbol_triggered(bool checked)
{
    Settings::instance()->saveSetting("View", "ShowWrapSymbol", checked);

    for (int i = 0; i < ui->documentsTab->count(); ++i) {
        Document *doc = qobject_cast<Document *>(ui->documentsTab->widget(i));
        if (doc) {
            doc->editor()->setShowWrapSymbol(checked);
        }
    }
}

void MainWindow::on_actionZoom_In_triggered()
{
    for (int i = 0; i < ui->documentsTab->count(); ++i) {
        Document *doc = qobject_cast<Document *>(ui->documentsTab->widget(i));
        if (doc) {
            doc->editor()->zoomIn();
        }
    }
}

void MainWindow::on_actionoom_Out_triggered()
{
    for (int i = 0; i < ui->documentsTab->count(); ++i) {
        Document *doc = qobject_cast<Document *>(ui->documentsTab->widget(i));
        if (doc) {
            doc->editor()->zoomOut();
        }
    }
}

void MainWindow::on_action_Restore_Default_Zoom_triggered()
{
    for (int i = 0; i < ui->documentsTab->count(); ++i) {
        Document *doc = qobject_cast<Document *>(ui->documentsTab->widget(i));
        if (doc) {
            doc->editor()->defaultZoom();
        }
    }
}

void MainWindow::on_action_Move_to_Other_View_triggered()
{
    if (!ui->documentsTab) {
        qDebug() << "documentsTab is null!";
        return;
    }

    if (m_moveToOtherView) {
        m_moveToOtherView->execute();
    } else {
        MoveToOtherView* m_moveToOtherView = new MoveToOtherView(ui->documentsTab, this);
        m_moveToOtherView->execute();
    }
    delete m_moveToOtherView;
}

void MainWindow::on_actionMove_to_a_New_View_triggered()
{
    if (!ui->documentsTab) {
        qDebug() << "documentsTab is null!";
        return;
    }

    if (m_moveToNewView) {
        m_moveToNewView->execute();
    } else {
        MoveToNewView* m_moveToNewView = new MoveToNewView(ui->documentsTab, this);
        m_moveToNewView->execute();
    }
    delete m_moveToNewView;
}

void MainWindow::on_action_Open_in_a_New_Window_triggered()
{
    if (!ui->documentsTab) {
        qDebug() << "documentsTab is null!";
        return;
    }

    if (m_openInNewWindow) {
        m_openInNewWindow->execute();
    } else {
        OpenInNewWindow* m_openInNewWindow = new OpenInNewWindow(ui->documentsTab, this);
        m_openInNewWindow->execute();
    }
    delete m_openInNewWindow;
}

void MainWindow::on_action_Word_wrap_triggered()
{
    if (QPlainTextEdit* currentEditor = qobject_cast<QPlainTextEdit*>(ui->documentsTab->currentWidget())) {
        m_wordWrap->toggle(currentEditor);
        bool checked = m_wordWrap->isWordWrapEnabled(currentEditor);
        ui->action_Word_wrap->setChecked(checked);
        Settings::instance()->saveSetting("View", "WordWrap", checked);
    } else {
        qDebug() << "No active text editor found to toggle word wrap.";
    }
}
// FIXME: Setting doesn't save
void MainWindow::toggleWordWrap() {
    // Get the current text editor
    if (QPlainTextEdit* currentEditor = qobject_cast<QPlainTextEdit*>(ui->documentsTab->currentWidget())) {
        // Use the WordWrap instance to toggle word wrap
        m_wordWrap->toggle(currentEditor);

        // Optionally update the UI action's checked state
        bool checked = m_wordWrap->isWordWrapEnabled(currentEditor);
        ui->action_Word_wrap->setChecked(checked);
        Settings::instance()->saveSetting("View", "WordWrap", checked);
    } else {
        qDebug() << "No active text editor found to toggle word wrap.";
    }
}

void MainWindow::onTabChanged(int currentIndex)
{
    // Skip updates if the change was triggered by toggling
    if (currentIndex == m_formerTabIndex) {
        return; // Do nothing; indices are already updated in the toggle logic
    }

    // Update former and current tab indices
    m_formerTabIndex = m_currentTabIndex;
    m_currentTabIndex = currentIndex;
}

void MainWindow::on_actionMath_Rendering_triggered(bool checked)
{
    Helpers::notImplemented(this);
    Settings::instance()->saveSetting("View", "MathRendering", checked);

    for (int i = 0; i < ui->documentsTab->count(); ++i) {
        Document *doc = qobject_cast<Document *>(ui->documentsTab->widget(i));
        if (doc) {
            doc->editor()->setShowMathRendering(checked);
        }
    }
}

void MainWindow::on_actionToggle_to_Former_Tab_triggered()
{
    // Ensure a valid former tab exists
    if (m_formerTabIndex != -1) {
        // Temporarily disconnect the currentChanged signal to avoid overwriting indices
        disconnect(ui->documentsTab, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);

        // Swap the tabs
        ui->documentsTab->setCurrentIndex(m_formerTabIndex);

        // Update indices after the toggle
        std::swap(m_currentTabIndex, m_formerTabIndex);

        // Reconnect the currentChanged signal
        connect(ui->documentsTab, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
    }
}

void MainWindow::on_action_Full_Screen_toggled(bool enabled)
{
    if (enabled) {
        // Enter full screen mode
        showFullScreen();
        Settings::instance()->saveSetting("View", "FullScreen", "true");
    } else {
        // Exit full screen mode
        showNormal();
        Settings::instance()->saveSetting("View", "FullScreen", "false");
    }

    // Update the action's checked state based on window state
    ui->action_Full_Screen->setChecked(isFullScreen());
}

/* Encoding */

void MainWindow::on_action_Interpret_as_UTF_8_triggered()
{
    CodeEditor* editor = dynamic_cast<CodeEditor*>(ui->documentsTab->currentWidget());
    Interpret_As_Utf_8::instance().execute(editor);
}

void MainWindow::on_actionInterpret_as_utf_8_without_BOM_triggered()
{
    CodeEditor* editor = dynamic_cast<CodeEditor*>(ui->documentsTab->currentWidget());
    Interpret_As_UTF_8_Without_BOM::instance().execute(editor);
}

void MainWindow::on_actionInterpret_as_16_BE_triggered()
{
    Document* doc = qobject_cast<Document*>(ui->documentsTab->currentWidget());
    CodeEditor* editor = doc->editor();
    Interpret_As_UTF_16_BE::instance().execute(editor);
}

void MainWindow::on_actionInterpret_as_16_LE_triggered()
{
    Document* doc = qobject_cast<Document*>(ui->documentsTab->currentWidget());
    CodeEditor* editor = doc->editor();
    InterpretAsUTF16LE::instance().execute(editor);
}

void MainWindow::on_actionInterpret_As_triggered()
{
    InterpreteAsDialog dialog(this); // Create the CCC with MainWindow as the parent

    if (dialog.exec() == QDialog::Accepted) {
        // OK clicked
        QString selectedItem = dialog.getSelectedItem();
        qDebug() << "Selected item:" << selectedItem;
        Document* doc = qobject_cast<Document*>(ui->documentsTab->currentWidget());
        CodeEditor* editor = doc->editor();
        if (!editor) {
            qFatal("No active editor");
            return;
        }
        if (selectedItem == "UTF-8") {
            Interpret_As_Utf_8::instance().execute(editor);
        }
        if (selectedItem == "UTF-7") {
            Interpret_As_Utf_7::instance().execute(editor);
        }
        if (selectedItem == "UTF-32LE") {
            Interpret_As_UTF_32_LE::instance().execute(editor);
        }
        if (selectedItem == "UTF-32BE") {
            Interpret_As_UTF_32_BE::instance().execute(editor);
        }
        if (selectedItem == "UTF-16LE") {
            InterpretAsUTF16LE::instance().execute(editor);
        }
        if (selectedItem == "UTF-16BE") {
            Interpret_As_UTF_16_BE::instance().execute(editor);
        }
        if (selectedItem == "UTF-16") {
            Interpret_As_UTF_16::instance().execute(editor);
        }
        if (selectedItem == "US-ASCII") {
            Interpret_As_US_ASCII::instance().execute(editor);
        }
        if (selectedItem == "TSCII") {
            Interpret_As_TSCII::instance().execute(editor);
        }
        if (selectedItem == "TIS-620") {
            Interpret_As_TIS_620::instance().execute(editor);
        }
        if (selectedItem == "TIS-620") {
            Interpret_As_Shift_JTS::instance().execute(editor);
        }
        if (selectedItem == "SCSU") {
            Interpret_As_SCSU::instance().execute(editor);
        }
        if (selectedItem == "KOI8-U") {
            Helpers::notImplemented(this);
            //Interpret_As_KOI8_U::instance().execute(editor);
        }
        if (selectedItem == "KOI8-R") {
            Helpers::notImplemented(this);
            Interpret_As_KOI8_R::instance().execute(editor);
        }
        if (selectedItem == "ISO-8859-9") {
            Interpret_As_ISO_8859_9::instance().execute(editor);
        }
        if (selectedItem == "ISO-8859-8") {
            Interpret_As_ISO_8859_8::instance().execute(editor);
        }
        if (selectedItem == "ISO-8859-7") {
            Interpret_As_ISO_8859_7::instance().execute(editor);
        }
        if (selectedItem == "ISO-8859-6") {
            Interpret_As_ISO_8859_6::instance().execute(editor);
        }
    }
}



































































































































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



void MainWindow::on_action_About_Notepad_triggered()
{
    AboutDialog dialog(this);
    dialog.exec();
}


void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this, tr("About Qt"));
}


