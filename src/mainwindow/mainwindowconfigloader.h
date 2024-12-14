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
    bool showAllCharacters() const;
    bool showIndentGuide() const;
    bool showWrapSymbol() const;
    bool wordWrap() const;
    bool mathRendering() const;
    bool fullScreen() const;
};

