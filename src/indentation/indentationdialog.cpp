#include <QSettings>
#include "indentationdialog.h"
#include "ui_indentationdialog.h"
#include "../settings.h"
#include "../document.h"
#include "../codeeditor.h"
#include "../mainwindow/helpers.h"

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
    QSettings settings("Remisa", "Notepad--");
    settings.beginGroup("IndentationSettings");
    QString value = settings.value("indentationOption", "Tabs").toString(); // Default to tabs if no value found
    int indentationSize = settings.value("indentationSize", 1).toInt(); // Default to 1 if no value found
    settings.endGroup();

    ui->tabs->setChecked(value == "Tabs");
    ui->spaces->setChecked(value == "Spaces");
    ui->number->setValue(indentationSize);
}

void IndentationDialog::saveSettings() {
    QSettings settings("Remisa", "Notepad--");  // Adjust organization and application name
    settings.beginGroup("IndentationSettings");
    settings.setValue("indentationOption", ui->tabs->isChecked() ? "Tabs" : "Spaces");
    settings.setValue("indentationSize", ui->number->value());
    settings.endGroup();
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
    settings->setValue("IndentationSettings/indentationOption", useTabs ? "Tabs" : "Spaces");
    settings->setValue("IndentationSettings/indentationSize", indentationWidth);

    Document* doc = qobject_cast<Document*>(parent());
    if (doc && doc->editor()) {
        doc->editor()->applyIndentation(useTabs, indentationWidth);
    }
    Helpers::notImplemented(this);
}

