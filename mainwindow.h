#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_action_Open_triggered();  // Slot for opening a file
    void on_action_Save_triggered();  // Slot for saving a file

    void on_actionSave_As_triggered();

private:
    Ui::MainWindow *ui;               // Pointer to the UI object
};

#endif // MAINWINDOW_H
