#include <QVBoxLayout>
#include "replacedialog.h"
#include "ui_replacedialog.h"

ReplaceDialog::ReplaceDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::ReplaceDialog) {
    ui->setupUi(this);

    const int move = 180;
    const int reduceHeight = 170;

    // Adjust the vertical positions of the buttons to move them up
    ui->findNext->move(ui->findNext->x(), ui->findNext->y() - move);
    ui->findPrevious->move(ui->findPrevious->x(), ui->findPrevious->y() - move);
    ui->replaceNext->move(ui->replaceNext->x(), ui->replaceNext->y() - move);
    ui->replacePrevious->move(ui->replacePrevious->x(), ui->replacePrevious->y() - move);
    ui->selectAll->move(ui->selectAll->x(), ui->selectAll->y() - move);
    ui->replaceAll->move(ui->replaceAll->x(), ui->replaceAll->y() - move);

    // Initially hide the advanced options and decrease the form and tabWidget height
    ui->groupBoxAdvanced->hide();
    resize(width(), height() - reduceHeight);

    // Connect advancedOptions checkbox to toggle advanced options visibility
    connect(ui->advancedOptions, &QCheckBox::toggled, this, &ReplaceDialog::toggleAdvancedOptions);
}

ReplaceDialog::~ReplaceDialog()
{
    delete ui;
}

QString ReplaceDialog::getReplaceText() const {
    return ui->replaceLineEdit->currentText();
}

bool ReplaceDialog::isMatchCaseChecked() const {
    return ui->matchCase->isChecked();
}

bool ReplaceDialog::isWholeWordsChecked() const {
    return ui->matchWholeWord->isChecked();
}

void ReplaceDialog::showDialog(QWidget *parent) {
    ReplaceDialog dialog(parent);
    dialog.setModal(true);
    dialog.exec();
}

void ReplaceDialog::on_replaceNext_clicked() {
    handleReplaceRequest(ui->replaceLineEdit->currentText(),
                        ui->matchCase->isChecked(),
                        ui->matchWholeWord->isChecked(),
                        selectedReplaceMode());  // Get current replace mode
}

void ReplaceDialog::on_advancedOptions_checkStateChanged(const Qt::CheckState &arg1)
{
    qDebug() << "on_advancedOptions_checkStateChanged";
}

void ReplaceDialog::on_replacePrevious_clicked() {
    handleReplaceRequest(ui->replaceLineEdit->currentText(),
                        ui->matchCase->isChecked(),
                        ui->matchWholeWord->isChecked(),
                        selectedReplaceMode());  // Get current replace mode
}

void ReplaceDialog::on_selectAll_clicked()
{
    qDebug() << "on_selectAll_clicked";
}

void ReplaceDialog::performReplace() {
    ReplaceMode mode = selectedReplaceMode();

    switch (mode) {
    case PlainText:
        qDebug() << "Performing Plain Text replace";
        // Implement plain text replace logic here
        break;
    case RegularExpression:
        qDebug() << "Performing Regular Expression replace";
        // Implement regex replace logic here
        break;
    case SpecialCharacters:
        qDebug() << "Performing Special Characters replace";
        // Implement special character replace logic here
        break;
    }
}

ReplaceDialog::ReplaceMode ReplaceDialog::selectedReplaceMode() const {
    if (ui->replacePlainText->isChecked()) {
        return PlainText;
    } else if (ui->replaceRegularExpression->isChecked()) {
        return RegularExpression;
    } else if (ui->replaceSpecialCharachters->isChecked()) {
        return SpecialCharacters;
    }
    return PlainText; // Default fallback
}

void ReplaceDialog::handleReplaceRequest(const QString& replaceText, bool matchCase, bool matchWholeWord, ReplaceMode mode) {
    switch (mode) {
    case PlainText:
        qDebug() << "Performing Plain Text replace for:" << replaceText;
        break;
    case RegularExpression:
        qDebug() << "Performing Regular Expression replace for:" << replaceText;
        break;
    case SpecialCharacters:
        qDebug() << "Performing Special Characters replace for:" << replaceText;
        break;
    }
    // Implement your actual replace logic here
}

void ReplaceDialog::onAdvancedOptionsToggled(bool checked) {
    ui->groupBoxAdvanced->setVisible(checked);
}

void ReplaceDialog::toggleAdvancedOptions(bool checked) {
    const int move = 180;
    const int increaseHeight = 170;

    if (checked) {
        // Move buttons down, resize form, and show advanced options
        ui->findNext->move(ui->findNext->x(), ui->findNext->y() + move);
        ui->findPrevious->move(ui->findPrevious->x(), ui->findPrevious->y() + move);
        ui->replaceNext->move(ui->replaceNext->x(), ui->replaceNext->y() + move);
        ui->replacePrevious->move(ui->replacePrevious->x(), ui->replacePrevious->y() + move);
        ui->selectAll->move(ui->selectAll->x(), ui->selectAll->y() + move);
        ui->replaceAll->move(ui->replaceAll->x(), ui->replaceAll->y() + move);

        resize(width(), height() + increaseHeight);

        ui->groupBoxAdvanced->show();
    } else {
        // Return to compact layout with buttons moved up and form resized
        ui->findNext->move(ui->findNext->x(), ui->findNext->y() - move);
        ui->findPrevious->move(ui->findPrevious->x(), ui->findPrevious->y() - move);
        ui->replaceNext->move(ui->replaceNext->x(), ui->replaceNext->y() - move);
        ui->replacePrevious->move(ui->replacePrevious->x(), ui->replacePrevious->y() - move);
        ui->selectAll->move(ui->selectAll->x(), ui->selectAll->y() - move);
        ui->replaceAll->move(ui->replaceAll->x(), ui->replaceAll->y() - move);

        resize(width(), height() - increaseHeight);

        ui->groupBoxAdvanced->hide();
    }
}



