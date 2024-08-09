#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "CodeEditor.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->documentsTab->removeTab(0);
    // Create a CodeEditor and add it to the first tab
    CodeEditor *editor = new CodeEditor(this);
    ui->documentsTab->addTab(editor, "Open a text file...");
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_action_Open_triggered()
{
    // Create a dummy message box
    QMessageBox::information(this, "Test Menu Click", "The Test menu item was clicked!");
}

