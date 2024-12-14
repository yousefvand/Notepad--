#include "toggletoformertab.h"

ToggleToFormerTab::ToggleToFormerTab(QTabWidget* tabWidget)
    : m_tabWidget(tabWidget),
    m_currentTabIndex(-1),
    m_formerTabIndex(-1),
    m_isToggleToFormerTabActive(false) {
    if (m_tabWidget && m_tabWidget->count() > 0) {
        m_currentTabIndex = m_tabWidget->currentIndex();
    }
}

void ToggleToFormerTab::updateTabIndex(int currentIndex) {
    if (currentIndex == m_currentTabIndex)
        return;

    m_formerTabIndex = m_currentTabIndex;
    m_currentTabIndex = currentIndex;
}

void ToggleToFormerTab::setToggleActive(bool active) {
    m_isToggleToFormerTabActive = active;
    if (m_isToggleToFormerTabActive) {
        toggle();
    }
}

void ToggleToFormerTab::toggle() {
    if (!m_tabWidget || m_formerTabIndex == -1) {
        return; // No former tab to toggle to
    }

    // Swap current and former indices and activate the former tab
    std::swap(m_currentTabIndex, m_formerTabIndex);
    m_tabWidget->setCurrentIndex(m_currentTabIndex);
}
