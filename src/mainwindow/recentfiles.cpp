#include <QFileDialog>
#include <QDebug>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include "recentfiles.h"
#include "../mainwindow.h"
#include "fileoperations.h"
#include "settings.h"

RecentFiles& RecentFiles::instance() {
    static RecentFiles instance;
    return instance;
}

RecentFiles::RecentFiles() {
    qDebug() << "Initializing RecentFiles...";
    // Initialize the recent files menu
    recentFilesMenu = new QMenu(tr("Recent Files"), m_mainWindow);

    // Create actions for "Open All" and "Clear"
    openAllAction = new QAction(tr("Open All Recent Files"), this);
    clearAction = new QAction(tr("Clear Recent Files"), this);

    // Connect actions to their respective slots
    connect(openAllAction, &QAction::triggered, this, &RecentFiles::openRecentFiles);
    connect(clearAction, &QAction::triggered, this, &RecentFiles::clearRecentFiles);

    loadRecentFiles();
    updateMenu();
}

void RecentFiles::addToRecentFiles(const QString& filePath) {
    if (filePath.isEmpty()) {
        qDebug() << "Skipping empty file path.";
        return;
    }

    if (m_recentFiles.contains(filePath)) {
        qDebug() << "File already in recent files list:" << filePath;
        return;
    }

    qDebug() << "Adding file to recent files:" << filePath;
    m_recentFiles.prepend(filePath);

    // Ensure the list doesn't exceed 10 entries
    if (m_recentFiles.size() > 10) {
        m_recentFiles.removeLast();
    }

    saveRecentFiles();
}

void RecentFiles::clearRecentFiles() {
    qDebug() << "Clearing recent files list...";
    m_recentFiles.clear();
    saveRecentFiles();
    qDebug() << "Recent files list cleared.";
}

void RecentFiles::openRecentFiles() {
    if (!m_fileOperations) {
        qWarning() << "FileOperations instance is not initialized!";
        return;
    }

    // Iterate in reverse over the recent files list
    for (auto it = m_recentFiles.crbegin(); it != m_recentFiles.crend(); ++it) {
        const QString& filePath = *it;
        if (QFile::exists(filePath)) {
            qDebug() << "Opening recent file:" << filePath;
            m_fileOperations->openDocument(filePath);  // Use instance of FileOperations
        } else {
            qDebug() << "Skipping non-existent file:" << filePath;
        }
    }
}

QMenu* RecentFiles::getRecentFilesMenu() const {
    return recentFilesMenu;
}

void RecentFiles::updateMenu() {
    recentFilesMenu->clear();

    for (const QString& filePath : m_recentFiles) {
        QString fileName = QFileInfo(filePath).fileName();
        QAction* fileAction = new QAction(fileName, this);

        connect(fileAction, &QAction::triggered, this, [this, filePath]() {
            m_mainWindow->getFileOperations()->openDocument(filePath);
        });


        recentFilesMenu->addAction(fileAction);
    }

    if (!m_recentFiles.isEmpty()) {
        recentFilesMenu->addSeparator();
        recentFilesMenu->addAction(openAllAction);
        recentFilesMenu->addAction(clearAction);
    }
}

void RecentFiles::loadRecentFiles() {
    qDebug() << "Loading recent files from QSettings...";
    QVariant value = Settings::instance().value("recentFiles");

    if (value.canConvert<QStringList>()) {
        m_recentFiles = value.toStringList();
        qDebug() << "Loaded recent files:" << m_recentFiles;
    } else {
        qDebug() << "No recent files found. Clearing list.";
        m_recentFiles.clear();
    }
}

void RecentFiles::saveRecentFiles() {
    qDebug() << "Saving recent files:" << m_recentFiles;

    // Serialize the QStringList into a JSON string
    QString jsonString = serializeToJson(m_recentFiles);

    // Save the JSON string to settings
    Settings::instance().setValue("recentFiles", jsonString);
    Settings::instance().sync();  // Flush changes to disk

    qDebug() << "Saved recent files:" << m_recentFiles;
}

QString RecentFiles::serializeToJson(const QStringList& list) const {
    QJsonArray jsonArray;
    for (const QString& str : list) {
        jsonArray.append(str);
    }
    QJsonDocument jsonDoc(jsonArray);
    return QString(jsonDoc.toJson(QJsonDocument::Compact));
}

QStringList RecentFiles::deserializeFromJson(const QString& json) const {
    QJsonDocument jsonDoc = QJsonDocument::fromJson(json.toUtf8());
    QStringList list;
    if (jsonDoc.isArray()) {
        QJsonArray jsonArray = jsonDoc.array();
        for (const QJsonValue& value : jsonArray) {
            list.append(value.toString());
        }
    }
    return list;
}

void RecentFiles::setFileOperations(FileOperations* fileOperations) {
    m_fileOperations = fileOperations;
}

void RecentFiles::updateRecentFilesMenu(QAction* recentFilesAction) {
    if (m_recentFiles.isEmpty()) {
        recentFilesAction->setVisible(false);  // Hide if no recent files
        return;
    }

    recentFilesAction->setVisible(true);  // Show if recent files exist

    QWidget *parentWidget = qobject_cast<QWidget *>(recentFilesAction->parent());
    QMenu *recentFilesMenu = new QMenu(tr("Recent Files"), parentWidget);

    recentFilesAction->setMenu(recentFilesMenu);

    for (const QString& filePath : m_recentFiles) {
        QAction* fileAction = recentFilesMenu->addAction(filePath);
        connect(fileAction, &QAction::triggered, recentFilesMenu,
                [this, filePath]() {
                    m_fileOperations->openDocument(filePath);
                });
    }


    recentFilesMenu->addSeparator();

    QAction* openAllAction = recentFilesMenu->addAction(tr("Open All Recent Files"));
    connect(openAllAction, &QAction::triggered, this, [this]() {
        openRecentFiles();
    });

    QAction* clearAction = recentFilesMenu->addAction(tr("Clear Recent Files"));
    connect(clearAction, &QAction::triggered, this, [this, recentFilesAction]() {
        clearRecentFiles();
        updateRecentFilesMenu(recentFilesAction);  // Refresh menu
    });
}
