#pragma once

#include "../mainwindow.h"

class MainWindowConfigLoader
{
public:
    explicit MainWindowConfigLoader(MainWindow *mainWindow);
    void loadMainWindowConfig();

private:
    MainWindow* m_mainWindow;

    bool showTabs() const;
    bool showSpaces() const;
    bool showEOL() const;
};

