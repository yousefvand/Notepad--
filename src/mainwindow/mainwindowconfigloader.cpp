#include "../settings.h"
#include "../mainwindow.h"
#include "../ui_mainwindow.h"
#include "src/ui_mainwindow.h"
#include "mainwindowconfigloader.h"

MainWindowConfigLoader::MainWindowConfigLoader(MainWindow *mainWindow) : m_mainWindow(mainWindow) {}

void MainWindowConfigLoader::loadMainWindowConfig() {
    if (m_mainWindow && m_mainWindow->getUi()) {
        m_mainWindow->getUi()->action_Show_Tabs->setChecked(showTabs());
        m_mainWindow->getUi()->actionShow_Spaces->setChecked(showSpaces());
        m_mainWindow->getUi()->actionShow_End_of_Lines->setChecked(showEOL());
    }
}

bool MainWindowConfigLoader::showTabs() const {
    return Settings::instance()->loadSetting("View", "ShowTabs", "false") == true;
}

bool MainWindowConfigLoader::showSpaces() const {
    return Settings::instance()->loadSetting("View", "ShowSpaces", "false") == true;
}

bool MainWindowConfigLoader::showEOL() const {
    return Settings::instance()->loadSetting("View", "ShowEOL", "false") == true;
}


