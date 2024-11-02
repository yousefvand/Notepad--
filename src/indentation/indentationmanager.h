#pragma once

#include <QObject>
#include <QAction>
#include <QActionGroup>
#include "../settings.h"
#include "indentationdialog.h"

class MainWindow;

class IndentationManager : public QObject {
    Q_OBJECT

public:
    IndentationManager(MainWindow* mainWindow, QObject* parent = nullptr);
    void setupIndentationMenu(QAction* customAction, QAction* defaultAction, QAction* smartIndentAction);
    void loadIndentationSetting();
    void saveIndentationSetting(const QString& setting);
    void openIndentationDialog();
    bool isTabsSelected() const;
    void setToDefaultIndentation();
    void loadSmartIndentSetting();
    void saveSmartIndentSetting(bool enabled);
    void openIndentationDialog(MainWindow* mainWindow);

public slots:
    void onIndentationSelected();
    void onSmartIndentTriggered(bool checked);

private:
    QAction* m_actionCustom;
    QAction* m_actionDefaultSetting;
    MainWindow* m_mainWindow;
    IndentationDialog* m_indentationDialog = nullptr;
};

