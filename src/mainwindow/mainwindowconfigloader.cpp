#include "../settings.h"
#include "../mainwindow.h"
#include "../ui_mainwindow.h"
#include "mainwindowconfigloader.h"

MainWindowConfigLoader::MainWindowConfigLoader(MainWindow *mainWindow) : m_mainWindow(mainWindow) {}

void MainWindowConfigLoader::loadMainWindowConfig() {
    if (m_mainWindow && m_mainWindow->getUi()) {
        m_mainWindow->getUi()->action_Show_Tabs->setChecked(showTabs());
        m_mainWindow->getUi()->actionShow_Spaces->setChecked(showSpaces());
        m_mainWindow->getUi()->actionShow_End_of_Lines->setChecked(showEOL());
        m_mainWindow->getUi()->actionShow_All_Characters->setChecked(showAllCharacters());
        m_mainWindow->getUi()->actionShow_Indent_Guide->setChecked(showIndentGuide());
        m_mainWindow->getUi()->actionShow_Wrap_Symbol->setChecked(showWrapSymbol());
        m_mainWindow->getUi()->action_Word_wrap->setChecked(wordWrap());
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

bool MainWindowConfigLoader::showAllCharacters() const {
    return Settings::instance()->loadSetting("View", "ShowAllCharacters", "false") == true;
}

bool MainWindowConfigLoader::showIndentGuide() const {
    return Settings::instance()->loadSetting("View", "ShowIndentGuide", "false") == true;
}

bool MainWindowConfigLoader::showWrapSymbol() const {
    return Settings::instance()->loadSetting("View", "ShowWrapSymbol", "false") == true;
}

bool MainWindowConfigLoader::wordWrap() const {
    return Settings::instance()->loadSetting("View", "WordWrap", "false") == true;
}




