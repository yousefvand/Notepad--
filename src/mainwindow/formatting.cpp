#include <QDebug>
#include <QStatusBar>
#include "formatting.h"
#include "../mainwindow.h"

Formatting::Formatting(MainWindow* mainWindow) : QObject(mainWindow), m_mainWindow(mainWindow) {
    m_actionGroup = new QActionGroup(this);
    m_actionGroup->setExclusive(true);  // Ensure only one action is checked
}

void Formatting::setupActions(QAction* windowsAction, QAction* unixAction, QAction* oldMacAction) {
    // Initialize actions
    m_windowsAction = windowsAction;
    m_unixAction = unixAction;
    m_oldMacAction = oldMacAction;

    // Add actions to the group
    m_actionGroup->addAction(m_windowsAction);
    m_actionGroup->addAction(m_unixAction);
    m_actionGroup->addAction(m_oldMacAction);

    // Connect actions to their respective callbacks
    connect(m_windowsAction, &QAction::triggered, this, &Formatting::onWindowsFormatSelected);
    connect(m_unixAction, &QAction::triggered, this, &Formatting::onUnixFormatSelected);
    connect(m_oldMacAction, &QAction::triggered, this, &Formatting::onOldMacFormatSelected);

    // Load the saved format and set the corresponding action checked
    QString savedFormat = loadEolFormat();
    if (savedFormat == "Windows") {
        m_windowsAction->setChecked(true);
        updateStatusBar("EOL format: Windows");
    } else if (savedFormat == "Unix") {
        m_unixAction->setChecked(true);
        updateStatusBar("EOL format: Unix");
    } else if (savedFormat == "OldMac") {
        m_oldMacAction->setChecked(true);
        updateStatusBar("EOL format: Old Mac");
    }
}

void Formatting::saveEolFormat(const QString& format) {
    QSettings settings("YourCompany", "YourApp");
    settings.setValue("eolFormat", format);
    qDebug() << "Saved EOL format:" << format;
}

QString Formatting::loadEolFormat() {
    QSettings settings("YourCompany", "YourApp");
    return settings.value("eolFormat", "Unix").toString();  // Default to Unix
}

void Formatting::updateStatusBar(const QString& message) {
    if (m_mainWindow) {
        m_mainWindow->statusBar()->showMessage(message, 2000);  // Display message for 2 seconds
    }
}

// Callback for Windows format
void Formatting::onWindowsFormatSelected() {
    saveEolFormat("Windows");
    updateStatusBar("EOL format changed to: Windows");
}

// Callback for Unix format
void Formatting::onUnixFormatSelected() {
    saveEolFormat("Unix");
    updateStatusBar("EOL format changed to: Unix");
}

// Callback for Old Mac format
void Formatting::onOldMacFormatSelected() {
    saveEolFormat("OldMac");
    updateStatusBar("EOL format changed to: Old Mac");
}
