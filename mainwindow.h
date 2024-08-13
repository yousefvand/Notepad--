#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Document;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_action_Open_triggered();
    void on_action_Save_triggered();
    void on_actionSave_As_triggered();
    void on_documentsTab_tabCloseRequested(int index); // Existing function

    void on_action_New_triggered();

    void on_actionC_3_triggered();

private:
    Ui::MainWindow *ui;
    void openDocument(const QString &filePath);
};

#endif // MAINWINDOW_H
