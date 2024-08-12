#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_action_Open_triggered();  // Slot for opening a file
    void on_action_Save_triggered();  // Slot for saving a file
    void on_actionSave_As_triggered(); // Slot for saving a file as

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
