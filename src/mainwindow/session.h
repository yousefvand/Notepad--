#pragma once

#include "fileoperations.h"
#include <QObject>
#include <QJsonArray>
#include <QJsonObject>

class MainWindow;

class Session : public QObject {
    Q_OBJECT

public:
    explicit Session(MainWindow* mainWindow, FileOperations* fileOperations);
    Session(MainWindow* mainWindow);

    void loadSession();
    void saveSession();

private:
    MainWindow* m_mainWindow;
    FileOperations* m_fileOperations;
    void removeUntitledDocumentIfEmpty();
    void loadTabFromSession(const QJsonObject& tabData);
};
