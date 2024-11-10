#include <QSettings>
#include "indentationdialog.h"
#include "ui_indentationdialog.h"
#include "../settings.h"
#include "../document.h"
#include "../codeeditor.h"
#include "../helpers.h"

IndentationDialog::IndentationDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::IndentationDialog)
{
    ui->setupUi(this);

    loadSettings();

    connect(ui->tabs, &QRadioButton::clicked, this, &IndentationDialog::onIndentationSelected);
    connect(ui->spaces, &QRadioButton::clicked, this, &IndentationDialog::onIndentationSelected);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &IndentationDialog::onOkClicked);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &IndentationDialog::reject);
}

IndentationDialog::~IndentationDialog()
{
    delete ui;
}

bool IndentationDialog::isTabsSelected() const {
    return ui->tabs->isChecked();
}

int IndentationDialog::getIndentationValue() const {
    return ui->number->value();
}

void IndentationDialog::onIndentationSelected() {
    // Your logic when the indentation is selected
    qDebug() << "Indentation option selected.";
}

void IndentationDialog::loadSettings() {
    QString indentationOption = Settings::instance()->loadSetting("Indentation", "Option", "Tabs").toString();
    int indentationSize = Settings::instance()->loadSetting("Indentation", "Size", 1).toInt();

    // Update the UI elements based on loaded settings
    ui->tabs->setChecked(indentationOption == "Tabs");
    ui->spaces->setChecked(indentationOption == "Spaces");
    ui->number->setValue(indentationSize);
}

void IndentationDialog::saveSettings() {
    Settings::instance()->saveSetting("Indentation", "Option", ui->tabs->isChecked() ? "Tabs" : "Spaces");
    Settings::instance()->saveSetting("Indentation", "Size", ui->number->value());
}

void IndentationDialog::onOkClicked() {
    saveSettings();  // Save settings when OK is clicked
    accept();
}

void IndentationDialog::setTabsChecked(bool checked) {
    ui->tabs->setChecked(checked);
}

bool IndentationDialog::isTabsChecked() const {
    return ui->tabs->isChecked();
}

void IndentationDialog::on_buttonBox_accepted()
{
    bool useTabs = ui->tabs->isChecked();
    int indentationWidth = ui->number->value();

    auto* settings = Settings::instance();
    settings->saveSetting("Indentation", "Option", useTabs ? "Tabs" : "Spaces");
    settings->saveSetting("Indentation", "Size", indentationWidth);

    Document* doc = qobject_cast<Document*>(parent());
    if (doc && doc->editor()) {
        doc->editor()->applyIndentation(useTabs, indentationWidth);
    }
    Helpers::notImplemented(this);
}


