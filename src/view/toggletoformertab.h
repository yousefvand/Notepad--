#pragma once

#include <QTabWidget>

class ToggleToFormerTab {
public:
    explicit ToggleToFormerTab(QTabWidget* tabWidget);

    void toggle();      // Perform the toggle operation
    void updateTabIndex(int currentIndex); // Update tab indices
    void setToggleActive(bool active);    // Set toggle state

private:
    QTabWidget* m_tabWidget;
    int m_currentTabIndex;
    int m_formerTabIndex;
    bool m_isToggleToFormerTabActive; // Toggle active state
};
