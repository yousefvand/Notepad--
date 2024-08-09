#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "CodeEditor.h"
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileInfo>

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
    // Open a file dialog to choose the file
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Text Files (*.txt);;All Files (*)"));

    if (!fileName.isEmpty()) {
        QFile file(fileName);  // Create a QFile object
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);  // Create a QTextStream for reading the file
            QString content = in.readAll();  // Read the entire content of the file

            // Get the active editor (the one in the current tab)
            CodeEditor *editor = qobject_cast<CodeEditor*>(ui->documentsTab->currentWidget());
            if (editor) {
                editor->setPlainText(content);  // Set the new content in the CodeEditor

                // Get the file name without the full path
                QString baseName = QFileInfo(fileName).fileName();

                // Set the tab title to the file name
                int currentIndex = ui->documentsTab->currentIndex();
                ui->documentsTab->setTabText(currentIndex, baseName);

                // Optionally, set the window title to the file name
                setWindowTitle(baseName);
            }

            file.close();  // Close the file after reading
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Cannot open file: ") + file.errorString());
        }
    }
}

void MainWindow::on_action_Save_triggered()
{
    // Get the active editor (the one in the current tab)
    CodeEditor *editor = qobject_cast<CodeEditor*>(ui->documentsTab->currentWidget());
    if (editor) {
        // Open a file dialog to choose where to save the file
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Text Files (*.txt);;All Files (*)"));

        if (!fileName.isEmpty()) {
            QFile file(fileName);  // Create a QFile object
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);  // Create a QTextStream for writing to the file
                out << editor->toPlainText();  // Write the content of the editor to the file
                file.close();  // Close the file after writing

                // Optionally, update the tab title to the new file name
                QString baseName = QFileInfo(fileName).fileName();
                int currentIndex = ui->documentsTab->currentIndex();
                ui->documentsTab->setTabText(currentIndex, baseName);

                // Optionally, set the window title to the file name
                setWindowTitle(baseName);
            } else {
                QMessageBox::warning(this, tr("Error"), tr("Cannot save file: ") + file.errorString());
            }
        }
    }
}
