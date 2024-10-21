#include "mainwindow.h"
#include "codeeditor.h"
#include "ui_mainwindow.h"
#include "document.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressBar>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QLabel>
#include <QTimer>
#include <QActionGroup>
#include <QMimeDatabase>
#include <QMimeType>
#include <QDir>
#include <QTabBar>
#include <QRegularExpression>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextCursor>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    qDebug() << "Tab widget pointer: " << ui->documentsTab;
    ui->documentsTab->setTabsClosable(true);

    QMenu *menuLanguage = ui->menu_Language;
    QMenu *menuZ = new QMenu("z", this);
    QAction *actionZ80 = new QAction("Z80", this);
    menuZ->addAction(actionZ80);
    menuLanguage->addMenu(menuZ);
    connect(actionZ80, &QAction::triggered, this, &MainWindow::onActionZ80Triggered);

    QActionGroup *group = new QActionGroup(this);
    group->setExclusive(true);  // Ensure only one action is checked at a time

    // Add actions to the group
    group->addAction(ui->actionWindows_Format);
    group->addAction(ui->action_Unix_OS_X_Format);
    group->addAction(ui->action_Old_Mac_Format);

    // Optionally, set a default action checked
    ui->action_Unix_OS_X_Format->setChecked(true);

    qDebug() << "Setting up UI and clearing existing tabs.";

    // Explicitly remove any existing tabs
    while (ui->documentsTab->count() > 0) {
        ui->documentsTab->removeTab(0);
    }

    ui->documentsTab->setTabsClosable(true);

    connect(ui->documentsTab, &QTabWidget::tabCloseRequested, this, &MainWindow::on_documentsTab_tabCloseRequested);

    qDebug() << "Tabs cleared, preparing to open files.";

    // Get arguments from argv and open files
    QStringList arguments = QCoreApplication::arguments();

    if (arguments.size() <=1) {
        Document *newDoc = new Document("", this);  // Empty file path for untitled document
        ui->documentsTab->addTab(newDoc, "Untitled Document");
        connectSignals(newDoc);
        ui->documentsTab->setCurrentWidget(newDoc);

        qDebug() << "Untitled Document created";
    } else {
        qDebug() << "Arguments passed: " << arguments;

        for (int i = 1; i < arguments.size(); ++i) {  // Skip argv(0), which is the app name
            QString filePath = arguments.at(i);
            qDebug() << "Opening document: " << filePath;
            openDocument(filePath);  // Open each file in a new tab
        }
    }

    // Set up signal connections for all existing tabs/documents
    for (int i = 0; i < ui->documentsTab->count(); ++i) {
        Document* doc = qobject_cast<Document*>(ui->documentsTab->widget(i));
        if (doc) {
            qDebug() << "Connecting signals for document at tab index:" << i;
            connectSignals(doc);  // Connect signals to each document
        }
    }

    qDebug() << "Initialization complete, emitting uiReady signal.";
    initialize();  // Initialize UI readiness
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::initialize() {
    QTimer::singleShot(100, this, [this]() {
        qDebug() << "Emitting uiReady signal after 100ms delay";
        qDebug() << "About to emit uiReady signal...";
        emit uiReady();
    });
}

void MainWindow::openDocument(const QString &filePath) {
    if (!QFile::exists(filePath)) {
        qDebug() << "Error: File does not exist: " << filePath;
        return;
    }

    QString fileName = QFileInfo(filePath).fileName();
    qDebug() << "Opening file: " << filePath;

    // Ensure the file can be opened
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error: Unable to open file: " << filePath;
        QMessageBox::warning(this, tr("Error"), tr("Failed to open the file."));
        return;
    }

    if (ui->documentsTab->count() > 0) {
        Document* doc = qobject_cast<Document*>(ui->documentsTab->widget(0));

        if (doc && isUntitledDocument(ui->documentsTab->tabText(0))) {
            QString content = doc->getEditorContent();  // Retrieve the editor content
            if (content.isEmpty()) {  // Check if the document is empty
                qDebug() << "Closing untitled and empty document.";
                ui->documentsTab->removeTab(0);
                doc->deleteLater();  // Schedule safe deletion
            } else {
                qDebug() << "Untitled document is not empty, skipping removal.";
            }
        }
    }

    // Create a new document and add it as a tab
    Document* newDoc = new Document(filePath, this);
    ui->documentsTab->addTab(newDoc, fileName);
    connectSignals(newDoc);
    ui->documentsTab->setCurrentWidget(newDoc);

    qDebug() << "Document added with file path: " << filePath << " and file name: " << fileName;
}

void MainWindow::on_action_Open_triggered() {
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Text Files (*.txt);;All Files (*)"));
    if (!filePath.isEmpty()) {
        openDocument(filePath);
    }
}

bool MainWindow::isUntitledDocument(const QString &title) {
    // Static QRegularExpression to avoid repeated creation
    static const QRegularExpression regex(
        R"((?:&?U&?n&?t&?i&?t&?l&?e&?d&?) (?:&?D&?o&?c&?u&?m&?e&?n&?t))");

    // Perform the regex match
    return regex.match(title).hasMatch();
}

void MainWindow::on_action_Save_triggered() {
    Document *doc = qobject_cast<Document *>(ui->documentsTab->currentWidget());
    if (doc) {
        int currentIndex = ui->documentsTab->currentIndex();
        if (currentIndex != -1) {
            QString title = ui->documentsTab->tabText(currentIndex);
            qDebug() << "Command save for Untitled Document, title is: " << title;
            if (isUntitledDocument(title)) {
                qDebug() << "Save As dialog for Untitled Document Save command, title is: " << title;
                QString filePath = QFileDialog::getSaveFileName(this, tr("Save File As"), QString(),
                                                                tr("Text Files (*.txt);;All Files (*)"));

                if (filePath.isEmpty()) return;
                doc->saveFileAs(filePath);
                return;
            }
        } else {
            return;
        }
        doc->saveFile();
    } else {
        QMessageBox::warning(this, tr("Error"), tr("No document to save."));
    }
}

void MainWindow::on_actionSave_As_triggered() {
    Document *doc = qobject_cast<Document *>(ui->documentsTab->currentWidget());
    if (doc) {
        QString filePath = QFileDialog::getSaveFileName(this, tr("Save File As"), "", tr("Text Files (*.txt);;All Files (*)"));
        if (!filePath.isEmpty()) {
            doc->saveFileAs(filePath);
            int tabIndex = ui->documentsTab->indexOf(doc);
            if (tabIndex != -1) {
                ui->documentsTab->setTabText(tabIndex, doc->fileName());
            }
        }
    } else {
        QMessageBox::warning(this, tr("Error"), tr("No document to save."));
    }
}

void MainWindow::on_action_New_triggered() {
    Document *newDoc = new Document("", this);
    ui->documentsTab->addTab(newDoc, "Untitled Document");
    connectSignals(newDoc);
    ui->documentsTab->setCurrentWidget(newDoc);
}

void MainWindow::on_action_Go_to_line_in_editor_triggered()
{
    Document *doc = qobject_cast<Document *>(ui->documentsTab->currentWidget());
    if (doc) {
        doc->goToLineNumberInEditor   ();
    } else {
        QMessageBox::warning(this, tr("Error"), tr("No document open."));
    }
}

void MainWindow::on_action_Go_to_line_in_text_triggered() {
    Document* doc = qobject_cast<Document*>(ui->documentsTab->currentWidget());
    if (doc) {
        doc->goToLineNumberInText(this);
    } else {
        QMessageBox::warning(this, tr("No Document"), tr("There is no document open."));
    }
}

void MainWindow::on_action_Close_triggered()
{
    int activeTabIndex = ui->documentsTab->currentIndex();  // Get the index of the active tab

    if (activeTabIndex != -1) {  // Check if there's an active tab
        on_documentsTab_tabCloseRequested(activeTabIndex);  // Reuse the existing close logic
    }
}

void MainWindow::on_actionC_lose_all_triggered() {
    closeAllDocuments();  // Call the method to close all documents
}

void MainWindow::closeAllDocuments() {
    int tabCount = ui->documentsTab->count();
    qDebug() << "Attempting to close all tabs. Current tab count:" << tabCount;

    QList<Document*> docsToClose;
    for (int i = 0; i < tabCount; ++i) {
        Document* doc = qobject_cast<Document*>(ui->documentsTab->widget(i));
        if (doc && doc->closeDocument()) {  // Check if the document can be closed
            docsToClose.append(doc);
        } else {
            qDebug() << "Skipped closing tab due to unsaved changes.";
        }
    }

    // Batch remove the collected tabs
    for (Document* doc : docsToClose) {
        int index = ui->documentsTab->indexOf(doc);
        if (index != -1) {
            ui->documentsTab->removeTab(index);
            doc->deleteLater();  // Safely delete the document
        }
    }

    qDebug() << "All tabs have been closed.";
}


void MainWindow::on_actionC_3_triggered() {
    Document *doc = qobject_cast<Document *>(ui->documentsTab->currentWidget());
    if (doc) {
        qDebug() << "Menu action triggered for C++ syntax highlighting";
        doc->applySyntaxHighlighter("C++");
    } else {
        QMessageBox::warning(this, tr("Error"), tr("No document to format."));
    }
}

void MainWindow::on_actionPython_triggered()
{
    Document *doc = qobject_cast<Document *>(ui->documentsTab->currentWidget());
    if (doc) {
        qDebug() << "Menu action triggered for Python syntax highlighting";
        doc->applySyntaxHighlighter("Python");
    } else {
        QMessageBox::warning(this, tr("Error"), tr("No document to format."));
    }
}

void MainWindow::on_actionSav_e_all_triggered() {
    int tabCount = ui->documentsTab->count();
    qDebug() << "Save All triggered. Total open tabs:" << tabCount;

    // Iterate over all open tabs
    for (int i = 0; i < tabCount; ++i) {
        Document *doc = qobject_cast<Document *>(ui->documentsTab->widget(i));
        if (doc && doc->isModified()) {
            qDebug() << "Saving modified document in tab:" << i;

            QString filePath = doc->filePath();

            // If the document doesn't have a file path, prompt the user to save it
            if (filePath.isEmpty()) {
                QString title = ui->documentsTab->tabText(i);  // Get the current tab title

                QString savePath = QFileDialog::getSaveFileName(
                    this,
                    tr("Save %1").arg(title),  // Dialog title with current tab title
                    QString(),                // Default path
                    tr("Text Files (*.txt);;All Files (*)")
                    );

                if (savePath.isEmpty()) {
                    qDebug() << "User canceled save for tab:" << i;
                    continue;  // Skip saving this document
                }

                // Update the document's file path
                doc->setFilePath(savePath);

                // Directly set the tab title to the new file name
                ui->documentsTab->setTabText(i, QFileInfo(savePath).fileName());
                qDebug() << "Updated tab title to:" << QFileInfo(savePath).fileName();
            }

            // Save the document
            doc->saveFile();
        }
    }

    qDebug() << "Save All completed.";
}

void MainWindow::on_actionCu_t_triggered()
{
    // TODO: Cut
}


void MainWindow::on_action_Copy_triggered()
{
    // TODO: Copy
}


void MainWindow::on_action_Paste_triggered()
{
    // TODO: Paste
}


void MainWindow::on_action_Undo_triggered()
{
    // TODO: Undo
}


void MainWindow::on_action_Redo_triggered()
{
    // TODO: Redo
}


void MainWindow::on_actionZoom_In_triggered()
{
    // TODO: Zoom In
}


void MainWindow::on_actionoom_Out_triggered()
{
    // TODO: Zoom Out
}


void MainWindow::on_action_Restore_Default_Zoom_triggered()
{
    // TODO: Restore Default Zoom
}


void MainWindow::on_action_Word_wrap_triggered()
{
    // TODO: Word Wrap
}

void MainWindow::onActionZ80Triggered()
{
    qDebug() << "Z80 action triggered!";
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QApplication::aboutQt();
}


void MainWindow::on_action_About_Notepad_triggered()
{
    // TODO: About Remisa Yousefvand
}

void MainWindow::on_actionOpen_Folder_triggered() {
    // Step 1: Open a folder selection dialog
    QString folderPath = QFileDialog::getExistingDirectory(this, tr("Select Folder"), QString(),
                                                           QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (folderPath.isEmpty()) {
        qDebug() << "No folder selected.";
        return;
    }

    // Step 2: Get all files from the selected folder
    QDir directory(folderPath);
    QStringList fileList = directory.entryList(QDir::Files);

    if (fileList.isEmpty()) {
        QMessageBox::information(this, tr("No Files Found"), tr("No files were found in the selected folder."));
        return;
    }

    // Step 3: Use QMimeDatabase to check MIME types and open text files
    QMimeDatabase mimeDatabase;
    bool textFilesFound = false;

    for (const QString &fileName : fileList) {
        QString filePath = directory.absoluteFilePath(fileName);

        // Get the MIME type of the file
        QMimeType mimeType = mimeDatabase.mimeTypeForFile(filePath);

        // Check if the MIME type is text/plain (or another text-related MIME type)
        if (mimeType.name().startsWith("text/")) {
            qDebug() << "Opening file with MIME type:" << mimeType.name();
            openDocument(filePath);  // This function opens the document in a new tab
            textFilesFound = true;
        } else {
            qDebug() << "Skipping file with MIME type:" << mimeType.name();
        }
    }

    if (!textFilesFound) {
        QMessageBox::information(this, tr("No Text Files Found"), tr("No text files were found in the selected folder based on MIME types."));
    }
}

void MainWindow::on_actionSa_ve_a_copy_as_triggered() {
    Document *doc = qobject_cast<Document *>(ui->documentsTab->currentWidget());
    doc->saveAcopyAs();
}

void MainWindow::setTabColor(int index, const QString &color) {
    if (index < 0 || index >= ui->documentsTab->count()) return;

    ui->documentsTab->tabBar()->setTabTextColor(index, QColor(color));
    ui->documentsTab->tabBar()->update();  // Force repaint
    qDebug() << "Set tab color to" << color << "for tab at index:" << index;
}

void MainWindow::connectSignals(Document *doc) {
    if (!doc) return;

    connect(doc->editor(), &QPlainTextEdit::modificationChanged, this, [this, doc](bool changed) {
        applyColorCoding(doc, changed);
    });

    connect(doc->worker(), &FileLoaderWorker::savingFinished, this, [this, doc]() {
        applyColorCoding(doc, false);
    });
}

void MainWindow::removeTabSafely(int index) {
    qDebug() << "Removing tab safely at index:" << index;

    // Ensure the index is still valid after any potential shifts
    if (index < 0 || index >= ui->documentsTab->count()) {
        qDebug() << "Cannot remove tab, invalid index: " << index;
        return;
    }

    // Get the document from the current index
    Document *doc = qobject_cast<Document *>(ui->documentsTab->widget(index));
    if (!doc) {
        qDebug() << "No document found at index:" << index << " for removal.";
        return;
    }

    // Remove the tab and schedule the document for safe deletion
    ui->documentsTab->removeTab(index);

    QTimer::singleShot(0, this, [doc]() {
        qDebug() << "Deleting document after tab closure.";
        doc->deleteLater();  // Safely delete the document
    });

    qDebug() << "Tab at index:" << index << " removed. Remaining tabs:" << ui->documentsTab->count();
}

void MainWindow::on_documentsTab_tabCloseRequested(int index) {
    static bool closingInProgress = false;  // Prevent double closing

    if (closingInProgress) {
        qDebug() << "Tab close already in progress. Ignoring.";
        return;
    }

    closingInProgress = true;

    // Use 'this' as the context object to ensure proper signal management.
    QTimer::singleShot(0, this, [this, index]() {
        if (index >= ui->documentsTab->count()) {
            qDebug() << "Invalid tab index: " << index;
            closingInProgress = false;
            return;
        }

        Document* doc = qobject_cast<Document*>(ui->documentsTab->widget(index));
        if (doc && doc->closeDocument()) {
            qDebug() << "Closing tab at index:" << index;
            ui->documentsTab->removeTab(index);
            doc->deleteLater();
        } else {
            qDebug() << "Tab close canceled.";
        }

        closingInProgress = false;
    });
}

void MainWindow::on_actionClose_all_BUT_current_document_triggered() {
    int activeTabIndex = ui->documentsTab->currentIndex();  // Get the index of the active tab
    int tabCount = ui->documentsTab->count();

    if (activeTabIndex == -1 || tabCount <= 1) {
        qDebug() << "No tabs to close or only one active tab.";
        return;
    }

    qDebug() << "Closing all tabs except the active tab at index:" << activeTabIndex;

    // Collect documents to close except the active one
    QList<Document*> docsToClose;
    for (int i = 0; i < tabCount; ++i) {
        if (i != activeTabIndex) {
            Document* doc = qobject_cast<Document*>(ui->documentsTab->widget(i));
            if (doc && doc->closeDocument()) {  // Check if the document can be closed
                docsToClose.append(doc);
            } else {
                qDebug() << "Skipped closing tab due to unsaved changes.";
            }
        }
    }

    // Batch remove the collected tabs
    for (Document* doc : docsToClose) {
        int index = ui->documentsTab->indexOf(doc);
        if (index != -1) {
            ui->documentsTab->removeTab(index);
            doc->deleteLater();  // Safely delete the document
        }
    }

    qDebug() << "All tabs except the active one have been closed.";
}

void MainWindow::on_actionSave_session_triggered() {
    QString filePath = QFileDialog::getSaveFileName(
        this, tr("Save Session"), "", tr("JSON Files (*.json);;All Files (*)"));

    if (filePath.isEmpty()) return;

    QJsonArray tabsArray;

    for (int i = 0; i < ui->documentsTab->count(); ++i) {
        Document* doc = qobject_cast<Document*>(ui->documentsTab->widget(i));
        if (doc) {
            QJsonObject tabData;

            // Ensure correct order: filePath -> isModified -> cursorPosition -> content
            tabData["filePath"] = doc->filePath();
            tabData["isModified"] = doc->isModified();
            tabData["cursorPosition"] = doc->editor()->textCursor().position();

            QString content = doc->isModified() ? doc->editor()->toPlainText() : "";
            tabData["content"] = content;  // Ensure content is added last

            tabsArray.append(tabData);
        }
    }

    QJsonObject sessionData;
    sessionData["activeTab"] = ui->documentsTab->currentIndex();
    sessionData["tabs"] = tabsArray;

    QJsonDocument doc(sessionData);
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    }
}

void MainWindow::applyColorCoding(Document* doc, bool isModified) {
    int index = ui->documentsTab->indexOf(doc);
    if (index == -1) return;  // Invalid tab index

    QColor tabColor = isModified ? Qt::red : QColor(0, 100, 0);  // Dark Green for saved

    ui->documentsTab->tabBar()->setTabTextColor(index, tabColor);
    qDebug() << "Applied color" << tabColor.name() << "to tab index:" << index;
}

void MainWindow::restoreCursorPosition(Document *doc, int position) {
    QTimer::singleShot(0, doc->editor(), [doc, position]() {
        QTextCursor cursor = doc->editor()->textCursor();
        cursor.setPosition(position);
        doc->editor()->setTextCursor(cursor);
        doc->editor()->ensureCursorVisible();
    });
}

void MainWindow::on_actionLoad_session_triggered() {
    QString filePath = QFileDialog::getOpenFileName(
        this, tr("Load Session"), "", tr("JSON Files (*.json);;All Files (*)"));

    if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return;

    QByteArray sessionData = file.readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(sessionData);
    QJsonObject rootObj = jsonDoc.object();

    closeAllDocuments();  // Close all existing tabs before restoring the session

    QJsonArray tabsArray = rootObj["tabs"].toArray();
    int activeTabIndex = rootObj["activeTab"].toInt();

    for (int i = 0; i < tabsArray.size(); ++i) {
        QJsonObject tabData = tabsArray[i].toObject();
        loadTabFromSession(tabData);
    }

    ui->documentsTab->setCurrentIndex(activeTabIndex);
}

void MainWindow::loadTabFromSession(const QJsonObject &tabData) {
    QString filePath = tabData["filePath"].toString();
    bool isModified = tabData["isModified"].toBool();
    int cursorPosition = tabData["cursorPosition"].toInt();
    QString content = tabData["content"].toString();

    qDebug() << "\n==== Restoring Tab from Session ====";
    qDebug() << "File Path:" << filePath;
    qDebug() << "Is Modified:" << isModified;
    qDebug() << "Cursor Position:" << cursorPosition;

    Document *newDoc = nullptr;
    QString tabTitle;

    // Case 1: Untitled Document (no path)
    if (filePath.isEmpty()) {
        newDoc = new Document("", this);
        newDoc->editor()->setPlainText(content);
        newDoc->setModified(isModified);
        tabTitle = "Untitled Document";
    }
    // Case 2: Modified Document with Path (use session content)
    else if (isModified) {
        newDoc = new Document(filePath, this);
        newDoc->editor()->setPlainText("");
        newDoc->editor()->appendPlainText(content);
        newDoc->setModified(true);
        tabTitle = newDoc->fileName();
    }
    // Case 3: Unmodified Document with Path (load from path)
    else {
        newDoc = new Document(filePath, this);
        tabTitle = newDoc->fileName();
    }

    int tabIndex = ui->documentsTab->addTab(newDoc, tabTitle);
    applyColorCoding(newDoc, isModified);
    ui->documentsTab->setCurrentIndex(tabIndex);

    // Restore the cursor only after the document loading is completed
    connect(newDoc->worker(), &FileLoaderWorker::loadingFinished, this, [this, newDoc, cursorPosition]() {
        qDebug() << "Worker thread finished loading document.";

        QTextCursor cursor = newDoc->editor()->textCursor();
        int docLength = newDoc->editor()->document()->characterCount();

        // Set the cursor to the saved position or move to the end if out of bounds
        if (cursorPosition >= 0 && cursorPosition <= docLength) {
            cursor.setPosition(cursorPosition);
            newDoc->editor()->setTextCursor(cursor);
            qDebug() << "Cursor successfully restored to position:" << cursor.position();
        } else {
            qDebug() << "Cursor position" << cursorPosition << "out of bounds, moving to end.";
            newDoc->editor()->moveCursor(QTextCursor::End);
        }

        // Ensure UI updates are applied
        newDoc->editor()->ensureCursorVisible();
        newDoc->editor()->update();
    });

    // Connect all required signals for the document
    connectSignals(newDoc);

    qDebug() << "Connected signals for tab with title:" << tabTitle;
    qDebug() << "==== Tab Restoration Complete ====";
}
