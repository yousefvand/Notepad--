#include <QVBoxLayout>
#include <QMessageBox>
#include <QSettings>
#include <QStringList>
#include "replacedialog.h"
#include "ui_replacedialog.h"
#include "../helpers.h"
#include "../settings.h"

ReplaceDialog::ReplaceDialog(QWidget* parent)
    : QDialog(parent),
    ui(new Ui::ReplaceDialog),
    m_searchOptions(new SearchOptions()),
    m_replace(nullptr)
{

    ui->setupUi(this);

    const int move = 220;
    const int reduceHeight = 210;

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

    m_searchOptions->findMethod = FindMethod::SimpleText; // default method

    ui->comboBoxFind->installEventFilter(this);
    ui->comboBoxReplace->installEventFilter(this);
}

ReplaceDialog::~ReplaceDialog()
{
    delete ui;
    delete m_replace;
}

bool ReplaceDialog::eventFilter(QObject *watched, QEvent *event) {
    if (watched == ui->comboBoxFind && event->type() == QEvent::Show) {
        populateComboBoxFind();
    } else if (watched == ui->comboBoxReplace && event->type() == QEvent::Show) {
        populateComboBoxReplace();
    }
    return QDialog::eventFilter(watched, event);
}

// Populate comboBoxFind with recent keywords from settings
void ReplaceDialog::populateComboBoxFind() {
    QStringList keywords = Settings::instance()->loadSetting("Find", "Keywords", QStringList()).toStringList();
    ui->comboBoxFind->clear();
    ui->comboBoxFind->addItems(keywords);
    ui->comboBoxFind->setCurrentText(""); // Ensure it starts empty for new input
}

// Populate comboBoxReplace with recent replace terms from settings
void ReplaceDialog::populateComboBoxReplace() {
    QStringList replaceWiths = Settings::instance()->loadSetting("Replace", "ReplaceWiths", QStringList()).toStringList();
    ui->comboBoxReplace->clear();
    ui->comboBoxReplace->addItems(replaceWiths);
    ui->comboBoxReplace->setCurrentText(""); // Ensure it starts empty for new input
}

void ReplaceDialog::saveKeyword(const QString& keyword)
{
    if (keyword.isEmpty()) return;
    QStringList keywords = Settings::instance()->loadSetting("Find", "Keywords", QStringList()).toStringList();

    if (!keywords.contains(keyword)) {
        keywords.prepend(keyword); // Add keyword to the top of history list
        if (keywords.size() > 10) {
            keywords = keywords.mid(0, 10); // Limit history to 10 items
        }
        Settings::instance()->saveSetting("Find", "Keywords", keywords);

        // Update combobox without triggering text change
        ui->comboBoxFind->blockSignals(true);
        ui->comboBoxFind->clear();
        ui->comboBoxFind->addItems(keywords);
        ui->comboBoxFind->blockSignals(false);
    }
}

void ReplaceDialog::saveReplaceWith(const QString& replaceWith) {
    if (replaceWith.isEmpty()) return;
    QStringList replaceWiths = Settings::instance()->loadSetting("Replace", "ReplaceWiths", QStringList()).toStringList();

    if (!replaceWiths.contains(replaceWith)) {
        replaceWiths.prepend(replaceWith); // Add replaceWith to the top of history list
        if (replaceWiths.size() > 10) {
            replaceWiths = replaceWiths.mid(0, 10); // Limit history to 10 items
        }
        Settings::instance()->saveSetting("Replace", "ReplaceWiths", replaceWiths);

        // Update combobox without triggering text change
        ui->comboBoxReplace->blockSignals(true);
        ui->comboBoxReplace->clear();
        ui->comboBoxReplace->addItems(replaceWiths);
        ui->comboBoxReplace->blockSignals(false);
    }
}

void ReplaceDialog::onAdvancedOptionsToggled(bool checked) {
    ui->groupBoxAdvanced->setVisible(checked);
}

void ReplaceDialog::toggleAdvancedOptions(bool checked) {
    const int move = 220;
    const int increaseHeight = 210;

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

Replace* ReplaceDialog::getReplace() {
    return m_replace;
}

SearchOptions* ReplaceDialog::getSearchOptions() {
    return m_searchOptions;
}

QString ReplaceDialog::getFindText() const {
    return ui->comboBoxFind->currentText();
}

QString ReplaceDialog::getReplaceText() const {
    return ui->comboBoxReplace->currentText();
}

bool ReplaceDialog::replacePlainTextSelected() const {
    return ui->replacePlainText->isChecked();
}

bool ReplaceDialog::replaceRegularExpressionSelected() const {
    return ui->replaceRegularExpression->isChecked();
}

bool ReplaceDialog::replaceSpecialCharachtersSelected() const {
    return ui->replaceSpecialCharachters->isChecked();
}

bool ReplaceDialog::isWholeWordsChecked() const {
    return ui->matchWholeWord->isChecked();
}

bool ReplaceDialog::isMatchCaseChecked() const {
    return ui->matchCase->isChecked();
}

bool ReplaceDialog::isAllTabsChecked() const {
    return ui->checkBoxAllTabs->isChecked();
}

void ReplaceDialog::on_findNext_clicked()
{
    if (!m_replace) return;

    QString keyword = ui->comboBoxFind->currentText().trimmed();
    if (keyword.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter a search keyword.");
        return;
    }

    saveKeyword(keyword);  // Save without modifying UI
    m_replace->setSearchOptions(*m_searchOptions);
    m_replace->findNext();
}

void ReplaceDialog::on_replaceNext_clicked()
{
    if (!m_replace) return;

    QString keyword = ui->comboBoxFind->currentText().trimmed();
    if (keyword.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter a search keyword.");
        return;
    }

    saveKeyword(keyword);  // Save without modifying UI

    QString replaceWith = ui->comboBoxReplace->currentText().trimmed();
    if (replaceWith.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter a replace keyword.");
        return;
    }

    saveReplaceWith(replaceWith);  // Save without modifying UI
    m_replace->setSearchOptions(*m_searchOptions);
    m_replace->replaceNext();
}

void ReplaceDialog::on_findPrevious_clicked()
{
    if (!m_replace) return;

    QString keyword = ui->comboBoxFind->currentText().trimmed();
    if (keyword.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter a search keyword.");
        return;
    }

    saveKeyword(keyword);  // Save without modifying UI
    m_replace->setSearchOptions(*m_searchOptions);
    m_replace->findPrevious();
}

void ReplaceDialog::on_replacePrevious_clicked()
{
    if (!m_replace) return;

    QString keyword = ui->comboBoxFind->currentText().trimmed();
    if (keyword.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter a search keyword.");
        return;
    }

    saveKeyword(keyword);  // Save without modifying UI

    QString replaceWith = ui->comboBoxReplace->currentText().trimmed();
    if (replaceWith.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter a replace keyword.");
        return;
    }

    saveReplaceWith(replaceWith);  // Save without modifying UI
    m_replace->setSearchOptions(*m_searchOptions);
    m_replace->replacePrevious();
}

void ReplaceDialog::on_selectAll_clicked()
{
    if (!m_replace) return;

    QString keyword = ui->comboBoxFind->currentText().trimmed();
    if (keyword.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter a search keyword.");
        return;
    }

    saveKeyword(keyword);  // Save without modifying UI
    m_replace->setSearchOptions(*m_searchOptions);
    m_replace->selectAll();
}

void ReplaceDialog::on_replaceAll_clicked()
{
    if (!m_replace) return;

    QString keyword = ui->comboBoxFind->currentText().trimmed();
    if (keyword.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter a search keyword.");
        return;
    }

    saveKeyword(keyword);  // Save without modifying UI

    QString replaceWith = ui->comboBoxReplace->currentText().trimmed();
    if (replaceWith.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter a replace keyword.");
        return;
    }

    saveReplaceWith(replaceWith);  // Save without modifying UI
    m_replace->setSearchOptions(*m_searchOptions);
    m_replace->replaceAll();
}

void ReplaceDialog::on_comboBoxFind_currentTextChanged(const QString &arg1)
{
    m_searchOptions->keyword = arg1;
}

void ReplaceDialog::on_comboBoxFind_currentIndexChanged(int index)
{
    m_searchOptions->keyword = ui->comboBoxFind->itemText(index);
}

void ReplaceDialog::on_comboBoxReplace_currentTextChanged(const QString &arg1)
{
    m_searchOptions->replaceText = arg1;
}

void ReplaceDialog::on_comboBoxReplace_currentIndexChanged(int index)
{
    m_searchOptions->replaceText = ui->comboBoxReplace->itemText(index);
}

void ReplaceDialog::on_replacePlainText_toggled(bool checked)
{
    if (checked) m_searchOptions->findMethod = FindMethod::SimpleText;
}

void ReplaceDialog::on_replaceRegularExpression_toggled(bool checked)
{
    if (checked) {
        m_searchOptions->findMethod = FindMethod::RegularExpression;
        if (!Helpers::isValidRegularExpression(ui->comboBoxFind->currentText())) {
            QMessageBox::critical(this, "Error", "Invalid RegularExpression.");
        }
    }
}

void ReplaceDialog::on_replaceSpecialCharachters_toggled(bool checked)
{
    if (checked) Helpers::notImplemented(this);
    if (checked) m_searchOptions->findMethod = FindMethod::SpecialCharacters;
}

void ReplaceDialog::on_matchWholeWord_toggled(bool checked)
{
    m_searchOptions->matchWholeWord = checked;
}

void ReplaceDialog::on_matchCase_toggled(bool checked)
{
    m_searchOptions->matchCase = checked;
}

void ReplaceDialog::on_checkBoxAllTabs_toggled(bool checked)
{
    if (checked) Helpers::notImplemented(this);
    m_searchOptions->allTabs = checked;
}

void ReplaceDialog::setEditor(CodeEditor* editor) {
    m_editor = editor;               // Store the editor
    if (m_replace) {                    // If m_replace already exists, clean it up
        delete m_replace;
    }
    m_replace = new Replace(editor);       // Initialize m_replace with the editor
}

void ReplaceDialog::showDialog(QWidget *parent) {
    ReplaceDialog* dialog = new ReplaceDialog(parent);
    dialog->setWindowModality(Qt::NonModal);
    dialog->show();
}


