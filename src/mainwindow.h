#pragma once

#include <QActionGroup>
#include <QRadioButton>
#include <QGroupBox>
#include <QMainWindow>
#include "document.h"
#include "find/finddialog.h"
#include "replace/replacedialog.h"
#include "helpers.h"
#include "mainwindow/formatting.h"
#include "mainwindow/fileoperations.h"
#include "mainwindow/textoperations.h"
#include "indentation/indentationmanager.h"
#include "systemfind/systemfinddialog.h"
#include "systemreplace/systemreplacedialog.h"
#include "systemsearchresultdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Session;
class Settings;
class RecentFiles;
class FileOperations;
class MainWindowConfigLoader;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    Ui::MainWindow* getUi() const;
    void connectSignals(Document* doc);
    FileOperations* getFileOperations() const;
    void setSmartIndentChecked(bool checked);
    bool isSmartIndentChecked() const;
    Document* getCurrentDocument();

protected:
    void closeEvent(QCloseEvent* event) override;

public slots:
    void openSearchResult(const QString &filePath, int lineNumber);

private slots:
    void on_action_New_triggered();
    void on_action_Open_triggered();
    void on_action_Save_triggered();
    void onActionZ80Triggered();

    void on_actionOpen_Folder_triggered();

    void on_actionSave_As_triggered();

    void on_actionSa_ve_a_copy_as_triggered();

    void on_actionSav_e_all_triggered();

    void on_action_Close_triggered();

    void on_actionC_lose_all_triggered();

    void on_actionClose_all_BUT_current_document_triggered();

    void on_actionLoad_session_triggered();

    void on_actionSave_session_triggered();

    void on_action_Print_triggered();

    void on_actionE_xit_triggered();

    void on_action_Undo_triggered();

    void on_action_Redo_triggered();

    void on_actionCu_t_triggered();

    void on_action_Copy_triggered();

    void on_action_Paste_triggered();

    void on_action_Delete_triggered();

    void on_actionSelect_All_triggered();

    void on_actionCopy_Full_Path_to_Clipboard_triggered();

    void on_actionCopy_File_Name_to_Clipboard_triggered();

    void on_actionCopy_Directory_to_Clipboard_triggered();

    void on_action_UPPERCASE_triggered();

    void on_action_lowercase_triggered();

    void on_actionDuplicate_Current_Line_triggered();

    void on_actionDelete_Current_Line_triggered();

    void on_action_Move_Line_Up_triggered();

    void on_actionMove_Line_Down_triggered();

    void on_action_Trim_Trailing_Space_triggered();

    void on_actionTrim_Leading_Space_triggered();

    void on_actionTrim_Leading_and_Trailing_Space_triggered();

    void on_action_EOL_to_Space_triggered();

    void on_actionTab_to_Space_triggered();

    void on_action_Space_to_TAB_all_triggered();

    void on_actionS_pace_to_Tab_Leading_triggered();

    void on_action_Default_Setting_triggered();

    void on_action_Custom_triggered();

    void on_smartIndent_triggered();

    void on_action_Find_triggered();

    void on_actionFind_Next_triggered();

    void on_actionFind_previoud_triggered();

    void on_action_Replace_triggered();

    void on_actionFind_System_triggered();

    void on_actionReplace_N_ext_triggered();

    void on_actionReplace_P_revious_triggered();

    void on_actionReplace_S_ystem_triggered();

    void on_actionGo_to_Line_in_Text_triggered();

    void on_actionGo_to_Line_in_Editor_triggered();

    void on_action_Show_Tabs_triggered(bool checked);

    void on_actionShow_Spaces_triggered(bool checked);

private:
    Ui::MainWindow* ui;
    FileOperations* fileOperations;
    TextOperations* textOperations;
    QScopedPointer<Session> session;
    Helpers helpers;
    Settings *settings;
    Formatting* formatting;
    RecentFiles* recentFiles;
    void applyColorCoding(Document* doc, bool isModified);
    void setActiveDocumentEditorInFindDialog();
    void setActiveDocumentEditorInReplaceDialog();
    void setupSearchResultDialogConnectionsForFind();
    void setupSearchResultDialogConnectionsForReplace();

    void setupIndentationMenu();
    QAction* action_Custom;
    QAction* action_Default_Setting;

    void saveIndentationSetting(const QString& setting);  // Save the selected option
    void loadIndentationSetting();  // Load and apply the saved setting
    IndentationManager* indentationManager;
    SystemFindDialog* m_systemFindDialog = nullptr;
    SystemReplaceDialog* m_systemReplaceDialog = nullptr;
    SystemSearchResultDialog* m_systemSearchResultDialog;
    FindDialog* findDialog;
    ReplaceDialog* replaceDialog;
    SearchOptions* m_searchOptions;
    Find* m_find = nullptr;
    MainWindowConfigLoader* m_mainWindowConfigLoader;
};
