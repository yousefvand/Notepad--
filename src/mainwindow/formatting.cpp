#include <QDebug>
#include <QStatusBar>
#include "formatting.h"
#include "../mainwindow.h"
#include "settings.h"
#include "helpers.h"
#include "../codeeditor.h"

Formatting::Formatting(QObject* parent, QTabWidget* documentsTab)
    : QObject(parent), m_documentsTab(documentsTab) {

    m_actionGroup = new QActionGroup(this);
    m_actionGroup->setExclusive(true);  // Only one action can be checked at a time
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

    // Connect actions to the unified callback
    connect(m_actionGroup, &QActionGroup::triggered, this, &Formatting::onEolFormatChanged);

    // Load the saved EOL format from settings and set the corresponding action checked
    QString savedFormat = Settings::instance()->value("eolFormat", "Unix").toString();

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

void Formatting::onEolFormatChanged() {
    QAction* selectedAction = m_actionGroup->checkedAction();
    QString format;

    if (selectedAction == m_windowsAction) {
        format = "Windows";
        applyEOL(Windows);
    } else if (selectedAction == m_unixAction) {
        format = "Unix";
        applyEOL(Unix);
    } else if (selectedAction == m_oldMacAction) {
        format = "OldMac";
        applyEOL(OldMac);
    }

    // Save the selected format to Settings
    Settings::instance()->setValue("eolFormat", format);

    // Update status bar
    updateStatusBar(QString("EOL format changed to: %1").arg(format));
}

void Formatting::updateStatusBar(const QString& message) {
    QMainWindow* mainWindow = qobject_cast<QMainWindow*>(parent());
    if (mainWindow && mainWindow->statusBar()) {
        mainWindow->statusBar()->showMessage(message, 2000);  // Show message for 2 seconds
    } else {
        qDebug() << "Warning: Status bar is not available.";
    }
}

void Formatting::applyEOL(EOLType eolType) {
    Document* doc = Helpers::getCurrentDocument(m_documentsTab);
    if (!doc) return;

    QPlainTextEdit* editor = doc->editor();
    QString eol;

    // Determine the EOL based on the selected option
    switch (eolType) {
    case Windows:
        eol = "\r\n";  // Windows EOL
        break;
    case Unix:
        eol = "\n";  // Unix EOL
        break;
    case OldMac:
        eol = "\r";  // Old Mac EOL
        break;
    }

    // Retrieve the current content
    QString content = editor->toPlainText();

    // Use a single replacement to convert all line breaks
    content.replace(QRegularExpression("\r\n|\r|\n"), eol);

    // Replace the content of the document in one go
    editor->clear();  // Clear the current content
    editor->setPlainText(content);  // Set the modified content

    // Mark the document as modified
    doc->setModified(true);
}




