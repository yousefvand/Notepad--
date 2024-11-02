#include <QVBoxLayout>
#include "searchdialog.h"
#include "ui_searchdialog.h"

SearchDialog::SearchDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::SearchDialog) {
    ui->setupUi(this);

    const int move = 180;
    const int reduceHeight = 170;

    // Adjust the vertical positions of the buttons to move them up
    ui->findNext->move(ui->findNext->x(), ui->findNext->y() - move);
    ui->findPrevious->move(ui->findPrevious->x(), ui->findPrevious->y() - move);
    ui->selectAll->move(ui->selectAll->x(), ui->selectAll->y() - move);

    // Initially hide the advanced options and decrease the form and tabWidget height
    ui->groupBoxAdvanced->hide();
    resize(width(), height() - reduceHeight);
    ui->tabWidget->resize(ui->tabWidget->width(), ui->tabWidget->height() - reduceHeight);

    // Connect advancedOptions checkbox to toggle advanced options visibility
    connect(ui->advancedOptions, &QCheckBox::toggled, this, &SearchDialog::toggleAdvancedOptions);
}

SearchDialog::~SearchDialog()
{
    delete ui;
}

QString SearchDialog::getSearchText() const {
    return ui->searchLineEdit->currentText();
}

bool SearchDialog::isMatchCaseChecked() const {
    return ui->matchCase->isChecked();
}

bool SearchDialog::isWholeWordsChecked() const {
    return ui->matchWholeWord->isChecked();
}

void SearchDialog::showDialog(QWidget *parent) {
    SearchDialog dialog(parent);
    dialog.setModal(true);
    dialog.exec();
}

void SearchDialog::on_findNext_clicked() {
    handleSearchRequest(ui->searchLineEdit->currentText(),
                        ui->matchCase->isChecked(),
                        ui->matchWholeWord->isChecked(),
                        selectedSearchMode());  // Get current search mode
}

void SearchDialog::on_advancedOptions_checkStateChanged(const Qt::CheckState &arg1)
{

}

void SearchDialog::on_findPrevious_clicked() {
    handleSearchRequest(ui->searchLineEdit->currentText(),
                        ui->matchCase->isChecked(),
                        ui->matchWholeWord->isChecked(),
                        selectedSearchMode());  // Get current search mode
}

void SearchDialog::on_selectAll_clicked()
{

}

void SearchDialog::performSearch() {
    SearchMode mode = selectedSearchMode();

    switch (mode) {
    case PlainText:
        qDebug() << "Performing Plain Text search";
        // Implement plain text search logic here
        break;
    case RegularExpression:
        qDebug() << "Performing Regular Expression search";
        // Implement regex search logic here
        break;
    case SpecialCharacters:
        qDebug() << "Performing Special Characters search";
        // Implement special character search logic here
        break;
    }
}

SearchDialog::SearchMode SearchDialog::selectedSearchMode() const {
    if (ui->searchPlainText->isChecked()) {
        return PlainText;
    } else if (ui->searchRegularExpression->isChecked()) {
        return RegularExpression;
    } else if (ui->searchSpecialCharachters->isChecked()) {
        return SpecialCharacters;
    }
    return PlainText; // Default fallback
}

void SearchDialog::handleSearchRequest(const QString& searchText, bool matchCase, bool matchWholeWord, SearchMode mode) {
    switch (mode) {
    case PlainText:
        qDebug() << "Performing Plain Text search for:" << searchText;
        break;
    case RegularExpression:
        qDebug() << "Performing Regular Expression search for:" << searchText;
        break;
    case SpecialCharacters:
        qDebug() << "Performing Special Characters search for:" << searchText;
        break;
    }
    // Implement your actual search logic here
}

void SearchDialog::onAdvancedOptionsToggled(bool checked) {
    ui->groupBoxAdvanced->setVisible(checked);
}

void SearchDialog::toggleAdvancedOptions(bool checked) {
    const int move = 180;
    const int increaseHeight = 170;

    if (checked) {
        // Move buttons down, resize form and tabWidget, and show advanced options
        ui->findNext->move(ui->findNext->x(), ui->findNext->y() + move);
        ui->findPrevious->move(ui->findPrevious->x(), ui->findPrevious->y() + move);
        ui->selectAll->move(ui->selectAll->x(), ui->selectAll->y() + move);

        resize(width(), height() + increaseHeight);
        ui->tabWidget->resize(ui->tabWidget->width(), ui->tabWidget->height() + increaseHeight);

        ui->groupBoxAdvanced->show();
    } else {
        // Return to compact layout with buttons moved up and form resized
        ui->findNext->move(ui->findNext->x(), ui->findNext->y() - move);
        ui->findPrevious->move(ui->findPrevious->x(), ui->findPrevious->y() - move);
        ui->selectAll->move(ui->selectAll->x(), ui->selectAll->y() - move);

        resize(width(), height() - increaseHeight);
        ui->tabWidget->resize(ui->tabWidget->width(), ui->tabWidget->height() - increaseHeight);

        ui->groupBoxAdvanced->hide();
    }
}



