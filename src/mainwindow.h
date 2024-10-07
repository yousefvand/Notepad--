#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void uiReady();

public slots:
    void closeTab(int index);

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

private:
    Ui::MainWindow *ui;
    void initialize();
    void openDocument(const QString &filePath);
    void closeAllDocuments();  // Declare closeAllDocuments method here
};

