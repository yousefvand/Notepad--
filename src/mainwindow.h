#pragma once

#include <QMainWindow>
#include "document.h"
#include "mainwindow/helpers.h"
#include "mainwindow/formatting.h"
#include "mainwindow/fileoperations.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Session;
class RecentFiles;
class FileOperations;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    Ui::MainWindow* getUi() const;
    void connectSignals(Document* doc);
    FileOperations* getFileOperations() const;

protected:
    void closeEvent(QCloseEvent* event) override;

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

private:
    Ui::MainWindow* ui;
    FileOperations* fileOperations;
    Session* session;
    Helpers helpers;
    Formatting* m_formatting;
    RecentFiles* recentFiles;
    void applyColorCoding(Document* doc, bool isModified);
};
