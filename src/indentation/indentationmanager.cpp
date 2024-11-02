#include <QDebug>
#include <QActionGroup>
#include "../mainwindow.h"
#include "indentationmanager.h"

IndentationManager::IndentationManager(MainWindow* mainWindow, QObject* parent)
    : QObject(parent), m_mainWindow(mainWindow) {}

void IndentationManager::setupIndentationMenu(QAction* customAction, QAction* defaultAction, QAction* smartIndentAction) {
    m_actionCustom = customAction;
    m_actionDefaultSetting = defaultAction;

    m_actionCustom->setCheckable(true);
    m_actionDefaultSetting->setCheckable(true);

    // Create an exclusive QActionGroup for radio behavior
    QActionGroup* group = new QActionGroup(this);
    group->setExclusive(true);
    group->addAction(m_actionCustom);
    group->addAction(m_actionDefaultSetting);

    connect(m_actionCustom, &QAction::triggered, this, [this]() {
        saveIndentationSetting("Custom");
    });
    connect(m_actionDefaultSetting, &QAction::triggered, this, [this]() {
        saveIndentationSetting("Default");
    });
    connect(smartIndentAction, &QAction::toggled, this, &IndentationManager::onSmartIndentTriggered);
    loadIndentationSetting();
}

void IndentationManager::saveIndentationSetting(const QString& setting) {
    auto* settings = Settings::instance();
    settings->setValue("IndentationSettings/indentationSetting", setting);
    qDebug() << "Saved indentation setting:" << setting;
}

void IndentationManager::loadIndentationSetting() {
    auto* settings = Settings::instance();
    QString savedSetting = settings->value("IndentationSettings/indentationSetting", "Default").toString();

    if (savedSetting == "Custom") {
        m_actionCustom->setChecked(true);
    } else {
        m_actionDefaultSetting->setChecked(true);
    }

    qDebug() << "Loaded indentation setting:" << savedSetting;
}

void IndentationManager::onIndentationSelected() {
    IndentationDialog dialog;
    if (dialog.exec() == QDialog::Accepted) {
        bool tabsSelected = dialog.isTabsSelected();
        int indentationValue = dialog.getIndentationValue();

        qDebug() << "Tabs Selected:" << tabsSelected;
        qDebug() << "Indentation Value:" << indentationValue;

        // Save the values to settings
        auto* settings = Settings::instance();
        settings->setValue("IndentationSettings/indentationOption", tabsSelected ? "Tabs" : "Spaces");
        settings->setValue("IndentationSettings/indentationSize", indentationValue);
    }
}

void IndentationManager::openIndentationDialog() {
    // Only create dialog once and reuse it
    if (!m_indentationDialog) {
        m_indentationDialog = new IndentationDialog(m_mainWindow);

        // Set up initial settings and connect OK/Cancel buttons only once
        connect(m_indentationDialog, &QDialog::accepted, this, [this]() {
            bool tabsSelected = m_indentationDialog->isTabsChecked();
            int indentationValue = m_indentationDialog->getIndentationValue();

            // Save the values to settings
            auto* settings = Settings::instance();
            settings->setValue("IndentationSettings/indentationOption", tabsSelected ? "Tabs" : "Spaces");
            settings->setValue("IndentationSettings/indentationSize", indentationValue);
        });

        connect(m_indentationDialog, &QDialog::rejected, m_indentationDialog, &QDialog::close);
    }

    // Update dialog's tab checkbox state before showing
    m_indentationDialog->setTabsChecked(isTabsSelected());
    m_indentationDialog->show();
}

bool IndentationManager::isTabsSelected() const {
    auto* settings = Settings::instance();  // Using the singleton settings class
    QString indentationOption = settings->value("IndentationSettings/indentationOption", "Tabs").toString();
    return indentationOption == "Tabs";  // Returns true if "Tabs" is the selected option
}

void IndentationManager::setToDefaultIndentation() {
    // Logic to apply the default indentation
    auto* settings = Settings::instance();
    settings->setValue("IndentationSettings/indentationSetting", "Default");

    // Ensure that the default setting is reflected in the UI
    if (m_actionDefaultSetting) {
        m_actionDefaultSetting->setChecked(true);
    }
    qDebug() << "Default indentation applied and saved in settings.";
}

void IndentationManager::loadSmartIndentSetting() {
    auto* settings = Settings::instance();
    bool isChecked = settings->value("IndentationSettings/smartIndentEnabled", false).toBool(); // Default to unchecked
    m_mainWindow->setSmartIndentChecked(isChecked);
}

void IndentationManager::saveSmartIndentSetting(bool enabled) {
    auto* settings = Settings::instance();
    settings->setValue("IndentationSettings/smartIndentEnabled", enabled);
    qDebug() << "Smart Indent setting saved as" << (enabled ? "enabled" : "disabled");
}

void IndentationManager::openIndentationDialog(MainWindow* mainWindow) {
    IndentationDialog dialog(mainWindow); // Use mainWindow as parent
    dialog.exec();
}

void IndentationManager::onSmartIndentTriggered(bool checked) {
    auto* settings = Settings::instance();
    settings->setValue("IndentationSettings/smartIndentEnabled", checked);
    qDebug() << "Smart Indent setting saved:" << checked;
}




