#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMenu>
#include <QAction>
#include "fileoperations.h"

class MainWindow;  // Forward declaration of MainWindow

class RecentFiles : public QObject {
    Q_OBJECT

public:
    static RecentFiles& instance();

    explicit RecentFiles(MainWindow* mainWindow, int capacity = 10);

    void initialize(FileOperations* fileOperations);
    void addToRecentFiles(const QString& filePath);
    void clearRecentFiles();
    void openRecentFiles();
    void setFileOperations(FileOperations* fileOperations);
    void loadRecentFiles();
    void saveRecentFiles();
    QMenu* getRecentFilesMenu() const;
    void updateRecentFilesMenu(QAction* recentFilesAction);

private:
    RecentFiles();
    MainWindow* m_mainWindow;
    QStringList m_recentFiles;
    int m_capacity;
    QMenu* recentFilesMenu;
    QAction* openAllAction;
    QAction* clearAction;
    FileOperations* m_fileOperations = nullptr;

    // Update the recent files menu
    void updateMenu();
    QString serializeToJson(const QStringList& list) const;  // Serialize helper
    QStringList deserializeFromJson(const QString& json) const;  // Deserialize helper
};
