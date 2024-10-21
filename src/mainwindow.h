#pragma once

#include "document.h"
#include <QMainWindow>
#include <QMenu>
#include <QSettings>
#include <QStringList>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void restoreCursorPosition(Document* doc, int position);

protected:
    void closeEvent(QCloseEvent *event) override;  // Override close event

signals:
    void uiReady();

public slots:
    void notifyUIReady() {
        emit uiReady();
    }


private slots:
    void on_action_Open_triggered();
    void on_action_Save_triggered();
    void on_actionSave_As_triggered();
    void on_documentsTab_tabCloseRequested(int index);
    void on_action_New_triggered();
    void on_action_Go_to_line_in_editor_triggered();
    void on_action_Go_to_line_in_text_triggered();
    void on_action_Close_triggered();
    void on_actionC_lose_all_triggered();
    void on_actionC_3_triggered();
    void on_actionPython_triggered();
    void onActionZ80Triggered();

    void on_actionSav_e_all_triggered();

    void on_actionCu_t_triggered();

    void on_action_Copy_triggered();

    void on_action_Paste_triggered();

    void on_action_Undo_triggered();

    void on_action_Redo_triggered();

    void on_actionZoom_In_triggered();

    void on_actionoom_Out_triggered();

    void on_action_Restore_Default_Zoom_triggered();

    void on_action_Word_wrap_triggered();

    void on_actionAbout_Qt_triggered();

    void on_action_About_Notepad_triggered();

    void on_actionOpen_Folder_triggered();

    void on_actionSa_ve_a_copy_as_triggered();

    void on_actionClose_all_BUT_current_document_triggered();

    void on_actionSave_session_triggered();

    void on_actionLoad_session_triggered();

    void on_action_Print_triggered();

    void openRecentFile(const QString &filePath);  // Open a file from recent files

    void clearRecentFiles();  // Clear all recent files

    void on_actionE_xit_triggered();

private:
    Ui::MainWindow *ui;
    void initialize();
    void openDocument(const QString &filePath);
    void closeAllDocuments();
    void removeTabSafely(int index);
    int findUntitledDocumentIndex();
    void setTabColor(int index, const QString& color);
    void connectSignals(Document* doc);
    bool isUntitledDocument(const QString &title);
    void applyColorCoding(Document* doc, bool isModified);
    void loadTabFromSession(const QJsonObject &tabData);
    QStringList recentFiles;  // List of recent files
    QSettings settings;  // Store recent files inside the application
    QMenu *recentFilesMenu = nullptr; // Pointer to the Recent Files submenu
    void addToRecentFiles(const QString &filePath);  // Add new file path
    void updateRecentFilesMenu(); // Refresh submenu dynamically
    void loadRecentFiles();  // Load recent files from settings
    void saveRecentFiles();  // Save recent files to settings
};

