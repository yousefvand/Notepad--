#include <QVBoxLayout>
#include "finddialog.h"
#include "ui_finddialog.h"

FindDialog::FindDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::FindDialog) {
    ui->setupUi(this);

    const int move = 220;
    const int reduceHeight = 210;

    // Adjust the vertical positions of the buttons to move them up
    ui->findNext->move(ui->findNext->x(), ui->findNext->y() - move);
    ui->findPrevious->move(ui->findPrevious->x(), ui->findPrevious->y() - move);
    ui->selectAll->move(ui->selectAll->x(), ui->selectAll->y() - move);

    // Initially hide the advanced options and decrease the form and tabWidget height
    ui->groupBoxAdvanced->hide();
    resize(width(), height() - reduceHeight);

    // Connect advancedOptions checkbox to toggle advanced options visibility
    connect(ui->advancedOptions, &QCheckBox::toggled, this, &FindDialog::toggleAdvancedOptions);
}

FindDialog::~FindDialog()
{
    delete ui;
}

QString FindDialog::getFindText() const {
    return ui->comboBoxFind->currentText();
}

bool FindDialog::isMatchCaseChecked() const {
    return ui->matchCase->isChecked();
}

bool FindDialog::isWholeWordsChecked() const {
    return ui->matchWholeWord->isChecked();
}

void FindDialog::showDialog(QWidget *parent) {
    FindDialog* dialog = new FindDialog(parent);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setModal(false);
    dialog->show();
}

void FindDialog::on_findNext_clicked() {
    handleFindRequest(ui->comboBoxFind->currentText(),
                        ui->matchCase->isChecked(),
                        ui->matchWholeWord->isChecked(),
                        selectedFindMode());  // Get current find mode
}

void FindDialog::on_advancedOptions_checkStateChanged(const Qt::CheckState &arg1)
{
    qDebug() << "on_advancedOptions_checkStateChanged";
}

void FindDialog::on_findPrevious_clicked() {
    handleFindRequest(ui->comboBoxFind->currentText(),
                        ui->matchCase->isChecked(),
                        ui->matchWholeWord->isChecked(),
                        selectedFindMode());  // Get current find mode
}

void FindDialog::on_selectAll_clicked()
{
    qDebug() << "on_selectAll_clicked";
}

void FindDialog::performFind() {
    FindMode mode = selectedFindMode();

    switch (mode) {
    case PlainText:
        qDebug() << "Performing Plain Text find";
        // Implement plain text find logic here
        break;
    case RegularExpression:
        qDebug() << "Performing Regular Expression find";
        // Implement regex find logic here
        break;
    case SpecialCharacters:
        qDebug() << "Performing Special Characters find";
        // Implement special character find logic here
        break;
    }
}

FindDialog::FindMode FindDialog::selectedFindMode() const {
    if (ui->findPlainText->isChecked()) {
        return PlainText;
    } else if (ui->findRegularExpression->isChecked()) {
        return RegularExpression;
    } else if (ui->findSpecialCharachters->isChecked()) {
        return SpecialCharacters;
    }
    return PlainText; // Default fallback
}

void FindDialog::handleFindRequest(const QString& findText, bool matchCase, bool matchWholeWord, FindMode mode) {
    switch (mode) {
    case PlainText:
        qDebug() << "Performing Plain Text find for:" << findText;
        break;
    case RegularExpression:
        qDebug() << "Performing Regular Expression find for:" << findText;
        break;
    case SpecialCharacters:
        qDebug() << "Performing Special Characters find for:" << findText;
        break;
    }
    // Implement your actual find logic here
}

void FindDialog::onAdvancedOptionsToggled(bool checked) {
    ui->groupBoxAdvanced->setVisible(checked);
}

void FindDialog::toggleAdvancedOptions(bool checked) {
    const int move = 220;
    const int increaseHeight = 210;

    if (checked) {
        // Move buttons down, resize form and tabWidget, and show advanced options
        ui->findNext->move(ui->findNext->x(), ui->findNext->y() + move);
        ui->findPrevious->move(ui->findPrevious->x(), ui->findPrevious->y() + move);
        ui->selectAll->move(ui->selectAll->x(), ui->selectAll->y() + move);

        resize(width(), height() + increaseHeight);

        ui->groupBoxAdvanced->show();
    } else {
        // Return to compact layout with buttons moved up and form resized
        ui->findNext->move(ui->findNext->x(), ui->findNext->y() - move);
        ui->findPrevious->move(ui->findPrevious->x(), ui->findPrevious->y() - move);
        ui->selectAll->move(ui->selectAll->x(), ui->selectAll->y() - move);

        resize(width(), height() - increaseHeight);

        ui->groupBoxAdvanced->hide();
    }
}



