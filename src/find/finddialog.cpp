#include <QVBoxLayout>
#include <QMessageBox>
#include "finddialog.h"
#include "ui_finddialog.h"
#include "../find/find.h"
#include "../helpers.h"

FindDialog::FindDialog(QWidget* parent)
    : QDialog(parent),
    ui(new Ui::FindDialog),
    m_searchOptions(new SearchOptions()),
    m_find(nullptr)
{

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

    m_searchOptions->findMethod = FindMethod::SimpleText; // default method
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

FindDialog::~FindDialog()
{
    delete ui;
    delete m_find;
}

QString FindDialog::getFindText() const {
    return ui->comboBoxFind->currentText();
}

bool FindDialog::findPlainTextSelected() const {
    return ui->findPlainText->isChecked();
}

bool FindDialog::findRegularExpressionSelected() const {
    return ui->findRegularExpression->isChecked();
}

bool FindDialog::findSpecialCharachtersSelected() const {
    return ui->findSpecialCharachters->isChecked();
}

bool FindDialog::isWholeWordsChecked() const {
    return ui->matchWholeWord->isChecked();
}

bool FindDialog::isMatchCaseChecked() const {
    return ui->matchCase->isChecked();
}

bool FindDialog::isAllTabsChecked() const {
    return ui->checkBoxAllTabs->isChecked();
}

void FindDialog::on_findNext_clicked() {
    if (!m_find) return;
    qDebug() << "Searching for keyword:" << m_searchOptions->keyword;

    m_find->setSearchOptions(*m_searchOptions);
    m_find->findNext();
}

void FindDialog::on_findPrevious_clicked()
{
    if (!m_find) return;

    m_find->setSearchOptions(*m_searchOptions);
    m_find->findPrevious();
}

void FindDialog::on_selectAll_clicked()
{
    if (!m_find) return;

    m_find->setSearchOptions(*m_searchOptions);
    m_find->selectAll();
}

void FindDialog::on_comboBoxFind_currentTextChanged(const QString &arg1)
{
    m_searchOptions->keyword = arg1;
}

void FindDialog::on_comboBoxFind_currentIndexChanged(int index)
{
    m_searchOptions->keyword = ui->comboBoxFind->itemText(index);
}

void FindDialog::on_findPlainText_toggled(bool checked)
{
    if (checked) m_searchOptions->findMethod = FindMethod::SimpleText;
}

void FindDialog::on_findRegularExpression_toggled(bool checked)
{
    if (checked) {
        m_searchOptions->findMethod = FindMethod::RegularExpression;
        if (!Helpers::isValidRegularExpression(ui->comboBoxFind->currentText())) {
            QMessageBox::critical(this, "Error", "Invalid RegularExpression.");
        }
    }
}

void FindDialog::on_findSpecialCharachters_toggled(bool checked)
{
    if (checked) m_searchOptions->findMethod = FindMethod::SpecialCharacters;
}

void FindDialog::on_matchWholeWord_toggled(bool checked)
{
    m_searchOptions->matchWholeWord = checked;
}

void FindDialog::on_matchCase_toggled(bool checked)
{
    m_searchOptions->matchCase = checked;
}

void FindDialog::on_checkBoxAllTabs_toggled(bool checked)
{
    m_searchOptions->allTabs = checked;
}




void FindDialog::setEditor(CodeEditor* editor) {
    m_editor = editor;               // Store the editor
    if (m_find) {                    // If m_find already exists, clean it up
        delete m_find;
    }
    m_find = new Find(editor);       // Initialize m_find with the editor
}






void FindDialog::showDialog(QWidget *parent) {
    FindDialog* dialog = new FindDialog(parent);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setModal(false);
    dialog->show();
}





