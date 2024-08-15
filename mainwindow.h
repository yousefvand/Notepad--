#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_action_Open_triggered();
    void on_action_Save_triggered();
    void on_actionSave_As_triggered();
    void on_documentsTab_tabCloseRequested(int index);
    void on_action_New_triggered();
    void on_actionC_3_triggered();
    void on_action_Go_to_line_in_text_triggered();
    void on_action_Go_to_line_in_editor_triggered();

    void on_action_Close_triggered();

    void on_actionC_lose_all_triggered();

private:
    Ui::MainWindow *ui;
    void openDocument(const QString &filePath);
};

#endif // MAINWINDOW_H
