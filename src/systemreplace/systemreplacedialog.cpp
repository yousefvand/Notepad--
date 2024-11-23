#include "systemreplacedialog.h"
#include "ui_systemreplacedialog.h"
#include "../settings.h"

SystemReplaceDialog::SystemReplaceDialog(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::SystemReplaceDialog),
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
    connect(ui->advancedOptions, &QCheckBox::toggled, this, &SystemReplaceDialog::toggleAdvancedOptions);

    m_searchOptions->findMethod = FindMethod::SimpleText; // default method

    ui->comboBoxFind->installEventFilter(this);
    ui->comboBoxReplace->installEventFilter(this);
    ui->comboBoxLocation->installEventFilter(this);
    ui->comboBoxPattern->installEventFilter(this);
}

SystemReplaceDialog::~SystemReplaceDialog()
{
    delete ui;
    delete m_replace;
}

bool SystemReplaceDialog::eventFilter(QObject *watched, QEvent *event) {
    if (watched == ui->comboBoxFind && event->type() == QEvent::Show) {
        populateComboBoxFind();
    } else if (watched == ui->comboBoxReplace && event->type() == QEvent::Show) {
        populateComboBoxReplace();
    } else if (watched == ui->comboBoxLocation && event->type() == QEvent::Show) {
        populateComboBoxLocation();
    } else if (watched == ui->comboBoxPattern && event->type() == QEvent::Show) {
        populateComboBoxPattern();
    }
    return QDialog::eventFilter(watched, event);
}

// Populate comboBoxFind with recent keywords from settings
void SystemReplaceDialog::populateComboBoxFind() {
    QStringList keywords = Settings::instance()->loadSetting("Find", "Keywords", QStringList()).toStringList();
    ui->comboBoxFind->clear();
    ui->comboBoxFind->addItems(keywords);
    ui->comboBoxFind->setCurrentText(""); // Ensure it starts empty for new input
}

// Populate comboBoxReplace with recent replace terms from settings
void SystemReplaceDialog::populateComboBoxReplace() {
    QStringList replaceWiths = Settings::instance()->loadSetting("Replace", "ReplaceWiths", QStringList()).toStringList();
    ui->comboBoxReplace->clear();
    ui->comboBoxReplace->addItems(replaceWiths);
    ui->comboBoxReplace->setCurrentText(""); // Ensure it starts empty for new input
}

void SystemReplaceDialog::populateComboBoxLocation() {
    QStringList locations = Settings::instance()->loadSetting("Location", "Keywords", QStringList()).toStringList();
    ui->comboBoxLocation->clear();
    ui->comboBoxLocation->addItems(locations);
    ui->comboBoxLocation->setCurrentText(""); // Ensure it starts empty for new input
}

void SystemReplaceDialog::populateComboBoxPattern() {
    QStringList patterns = Settings::instance()->loadSetting("Pattern", "Keywords", QStringList()).toStringList();
    ui->comboBoxPattern->clear();
    ui->comboBoxPattern->addItems(patterns);
    ui->comboBoxPattern->setCurrentText(""); // Ensure it starts empty for new input
}

void SystemReplaceDialog::saveKeyword(const QString& keyword)
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

void SystemReplaceDialog::saveReplaceWith(const QString& replaceWith) {
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

void SystemReplaceDialog::saveLocation(const QString& location)
{
    if (location.isEmpty()) return;
    QStringList locations = Settings::instance()->loadSetting("Location", "Keywords", QStringList()).toStringList();

    if (!locations.contains(location)) {
        locations.prepend(location); // Add keyword to the top of history list
        if (locations.size() > 10) {
            locations = locations.mid(0, 10); // Limit history to 10 items
        }
        Settings::instance()->saveSetting("Location", "Keywords", locations);

        // Update combobox without triggering text change
        ui->comboBoxLocation->blockSignals(true);
        ui->comboBoxLocation->clear();
        ui->comboBoxLocation->addItems(locations);
        ui->comboBoxLocation->blockSignals(false);
    }
}

void SystemReplaceDialog::savePattern(const QString& pattern)
{
    if (pattern.isEmpty()) return;
    QStringList patterns = Settings::instance()->loadSetting("Pattern", "Keywords", QStringList()).toStringList();

    if (!patterns.contains(pattern)) {
        patterns.prepend(pattern); // Add keyword to the top of history list
        if (patterns.size() > 10) {
            patterns = patterns.mid(0, 10); // Limit history to 10 items
        }
        Settings::instance()->saveSetting("Pattern", "Keywords", patterns);

        // Update combobox without triggering text change
        ui->comboBoxPattern->blockSignals(true);
        ui->comboBoxPattern->clear();
        ui->comboBoxPattern->addItems(patterns);
        ui->comboBoxPattern->blockSignals(false);
    }
}

void SystemReplaceDialog::onAdvancedOptionsToggled(bool checked) {
    ui->groupBoxAdvanced->setVisible(checked);
}

void SystemReplaceDialog::toggleAdvancedOptions(bool checked) {
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













void SystemReplaceDialog::on_findNext_clicked()
{

}

