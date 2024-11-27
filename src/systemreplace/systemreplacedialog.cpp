#include <QMessageBox>
#include <QFileDialog>
#include <QDirIterator>
#include <QMimeDatabase>
#include "../helpers.h"
#include "../settings.h"
#include "systemreplacedialog.h"
#include "ui_systemreplacedialog.h"
#include "../search/filesearchworker.h"

SystemReplaceDialog::SystemReplaceDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::SystemReplaceDialog)
    , m_searchOptions(new SearchOptions()), m_files()
    , m_processedFiles(0), m_find(nullptr), m_systemSearchResultDialog(nullptr)
{
    ui->setupUi(this);

    connect(this, &SystemReplaceDialog::updateProgress, this, &SystemReplaceDialog::updateProgressDisplay);

    const int move = 220;
    const int reduceHeight = 210;

    // Adjust the vertical positions of the buttons to move them up
    ui->findNext->move(ui->findNext->x(), ui->findNext->y() - move);
    ui->findPrevious->move(ui->findPrevious->x(), ui->findPrevious->y() - move);
    ui->replaceNext->move(ui->replaceNext->x(), ui->replaceNext->y() - move);
    ui->replacePrevious->move(ui->replacePrevious->x(), ui->replacePrevious->y() - move);
    ui->selectAll->move(ui->selectAll->x(), ui->selectAll->y() - move);
    ui->replaceAll->move(ui->replaceAll->x(), ui->replaceAll->y() - move);
    ui->m_statusLabel->move(ui->m_statusLabel->x(), ui->m_statusLabel->y() - move);
    ui->m_progressBar->move(ui->m_progressBar->x(), ui->m_progressBar->y() - move);

    // Initially hide the advanced options and decrease the form and tabWidget height
    ui->groupBoxAdvanced->hide();
    resize(width(), height() - reduceHeight);

    // Connect advancedOptions checkbox to toggle advanced options visibility
    connect(ui->advancedOptions, &QCheckBox::toggled, this, &SystemReplaceDialog::toggleAdvancedOptions);

    ui->comboBoxFind->installEventFilter(this);
    ui->comboBoxReplace->installEventFilter(this);
    ui->comboBoxLocation->installEventFilter(this);
    ui->comboBoxPattern->installEventFilter(this);
}

SystemReplaceDialog::~SystemReplaceDialog()
{
    delete ui;
    delete m_replace;
    delete m_searchOptions;
    delete m_systemSearchResultDialog;
}

void SystemReplaceDialog::cleanupResources() {
    qInfo() << "Cleaning up resources. Closing window...";
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

void SystemReplaceDialog::saveHistory() {
    QString keyword = ui->comboBoxFind->currentText();
    if (keyword.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter a search keyword.");
        return;
    }
    saveKeyword(keyword);

    QString replace = ui->comboBoxReplace->currentText();
    if (replace.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter a replace keyword.");
        return;
    }
    saveReplaceWith(replace);

    QString location = ui->comboBoxLocation->currentText();
    if (location.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select a location.");
        return;
    }
    saveLocation(location);

    QString pattern = ui->comboBoxPattern->currentText();
    if (!pattern.isEmpty()) {
        savePattern(pattern);
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
        ui->m_statusLabel->move(ui->m_statusLabel->x(), ui->m_statusLabel->y() + move);
        ui->m_progressBar->move(ui->m_progressBar->x(), ui->m_progressBar->y() + move);

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
        ui->m_statusLabel->move(ui->m_statusLabel->x(), ui->m_statusLabel->y() - move);
        ui->m_progressBar->move(ui->m_progressBar->x(), ui->m_progressBar->y() - move);

        resize(width(), height() - increaseHeight);

        ui->groupBoxAdvanced->hide();
    }
}

void SystemReplaceDialog::on_pushButtonBrowse_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"));
    if (!dir.isEmpty()) {
        ui->comboBoxLocation->addItem(dir);
        ui->comboBoxLocation->setCurrentText(dir);
    }
}

void SystemReplaceDialog::startSearchNext(const SearchOptions& options) {
    if (!m_searchOptions) {
        m_searchOptions = new SearchOptions();
    }
    *m_searchOptions = options;

    // Reset processed files count
    m_processedFiles = 0;
    m_files.clear();
    ui->m_progressBar->setValue(0);

    // Compile the regex pattern if it's not empty
    QRegularExpression regex;
    if (!m_searchOptions->pattern.isEmpty()) {
        regex.setPattern(m_searchOptions->pattern);
        if (!regex.isValid()) {
            qWarning() << "Invalid regex pattern: " << regex.errorString();
            return;
        }
        qDebug() << "Using pattern: " << m_searchOptions->pattern;
    } else {
        qDebug() << "No pattern provided. All files will be considered.";
    }

    // Prepare flags for QDirIterator
    QDirIterator::IteratorFlags flags = m_searchOptions->includeSubdirectories
                                            ? QDirIterator::Subdirectories
                                            : QDirIterator::NoIteratorFlags;

    QDir::Filters dirFilters = QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot;

    // Process files in the directory
    QMimeDatabase mimeDb;
    QDirIterator it(m_searchOptions->location, dirFilters, flags);

    while (it.hasNext()) {
        QString filePath = it.next();
        QString fileName = QFileInfo(filePath).fileName();
        QMimeType mimeType = mimeDb.mimeTypeForFile(filePath);

        // Match MIME type and (if present) regex pattern
        if (mimeType.name().startsWith("text/") &&
            (m_searchOptions->pattern.isEmpty() || regex.match(fileName).hasMatch())) {
            qInfo() << "Matched file: " << filePath;
            processFile(filePath);
        }
    }

    qInfo() << "Search completed. Total processed files: " << m_processedFiles;
    ui->m_progressBar->setMaximum(m_processedFiles);
}

void SystemReplaceDialog::on_replaceNext_clicked()
{
    m_searchOptions->role = Role::ReplaceNext;
    UpdateSearchOptions();
    saveHistory();
    if (!m_systemSearchResultDialog) {
        startSearchNext(*m_searchOptions);
    } else {
        m_systemSearchResultDialog->traverseAndReplaceKeywords(false);
    }
    showResultDialog();
}

void SystemReplaceDialog::on_findPrevious_clicked()
{
    m_searchOptions->role = Role::FindPrevious;
    UpdateSearchOptions();
    saveHistory();
    if (!m_systemSearchResultDialog) {
        startSearchPrevious(*m_searchOptions);
    } else {
        m_systemSearchResultDialog->traverseKeywords(true);
    }
    showResultDialog();
}

void SystemReplaceDialog::startSearchPrevious(const SearchOptions& options) {
    // Store the search options
    if (!m_searchOptions) {
        m_searchOptions = new SearchOptions();
    }
    *m_searchOptions = options;

    // Reset processed files count
    m_processedFiles = 0;
    m_files.clear();
    ui->m_progressBar->setValue(0);

    // Compile the regex pattern if it's not empty
    QRegularExpression regex;
    if (!m_searchOptions->pattern.isEmpty()) {
        regex.setPattern(m_searchOptions->pattern);
        if (!regex.isValid()) {
            qWarning() << "Invalid regex pattern: " << regex.errorString();
            return;
        }
        qDebug() << "Using pattern: " << m_searchOptions->pattern;
    } else {
        qDebug() << "No pattern provided. All files will be considered.";
    }

    // Prepare flags for QDirIterator
    QDirIterator::IteratorFlags flags = m_searchOptions->includeSubdirectories
                                            ? QDirIterator::Subdirectories
                                            : QDirIterator::NoIteratorFlags;

    QDir::Filters dirFilters = QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot;

    // Collect all matching files in a QList
    QMimeDatabase mimeDb;
    QList<QString> matchingFiles;

    QDirIterator it(m_searchOptions->location, dirFilters, flags);
    while (it.hasNext()) {
        QString filePath = it.next();
        QString fileName = QFileInfo(filePath).fileName();
        QMimeType mimeType = mimeDb.mimeTypeForFile(filePath);

        // Match MIME type and (if present) regex pattern
        if (mimeType.name().startsWith("text/") &&
            (m_searchOptions->pattern.isEmpty() || regex.match(fileName).hasMatch())) {
            matchingFiles.append(filePath);
        }
    }

    // Reverse iterate through the list of matching files
    for (auto it = matchingFiles.rbegin(); it != matchingFiles.rend(); ++it) {
        QString filePath = *it;
        qDebug() << "Processing file (reverse): " << filePath;
        processFile(filePath);
    }

    qInfo() << "Search completed. Total processed files: " << m_processedFiles;
    ui->m_progressBar->setMaximum(m_processedFiles);
}

void SystemReplaceDialog::on_replacePrevious_clicked()
{
    m_searchOptions->role = Role::ReplacePrevious;
    UpdateSearchOptions();
    saveHistory();
    if (!m_systemSearchResultDialog) {
        startSearchPrevious(*m_searchOptions);
    } else {
        m_systemSearchResultDialog->traverseAndReplaceKeywords(true);
    }
    showResultDialog();
}

void SystemReplaceDialog::selectAll(const SearchOptions& options) {
    // Store the search options
    if (!m_searchOptions) {
        m_searchOptions = new SearchOptions();
    }
    *m_searchOptions = options;

    // Reset processed files count
    m_processedFiles = 0;
    m_files.clear();
    ui->m_progressBar->setValue(0);

    // Compile the regex pattern from SearchOptions->pattern
    QRegularExpression regex;
    if (!m_searchOptions->pattern.isEmpty()) {
        regex.setPattern(m_searchOptions->pattern.replace("\\\\", "\\"));
        if (!regex.isValid()) {
            qWarning() << "Invalid regex pattern: " << regex.errorString();
            return;
        }
        qDebug() << "Using pattern: " << m_searchOptions->pattern;
    } else {
        qDebug() << "No pattern provided. Selecting all files.";
    }

    // Prepare flags for QDirIterator to include hidden subdirectories
    QDirIterator::IteratorFlags flags = m_searchOptions->includeSubdirectories
                                            ? QDirIterator::Subdirectories
                                            : QDirIterator::NoIteratorFlags;

    QDir::Filters dirFilters = QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot;

    // Process files in the directory
    QMimeDatabase mimeDb;
    QDirIterator it(m_searchOptions->location, dirFilters, flags);

    while (it.hasNext()) {
        QString filePath = it.next();
        QString fileName = QFileInfo(filePath).fileName();
        QMimeType mimeType = mimeDb.mimeTypeForFile(filePath);

        // Match MIME type and pattern (if provided)
        if (mimeType.name().startsWith("text/") &&
            (m_searchOptions->pattern.isEmpty() || regex.match(fileName).hasMatch())) {
            qInfo() << "Selected file:  " << filePath;
            processFile(filePath);
        }
    }

    qInfo() << "Select All completed. Total files processed: " << m_processedFiles;
}

void SystemReplaceDialog::replaceAll(const SearchOptions& options) {
    // Store the search options
    if (!m_searchOptions) {
        m_searchOptions = new SearchOptions();
    }
    *m_searchOptions = options;

    // Reset processed files count
    m_processedFiles = 0;
    m_files.clear();
    ui->m_progressBar->setValue(0);

    // Compile the regex pattern from SearchOptions->pattern
    QRegularExpression regex;
    if (!m_searchOptions->pattern.isEmpty()) {
        regex.setPattern(m_searchOptions->pattern.replace("\\\\", "\\"));
        if (!regex.isValid()) {
            qWarning() << "Invalid regex pattern: " << regex.errorString();
            return;
        }
        qDebug() << "Using pattern: " << m_searchOptions->pattern;
    } else {
        qDebug() << "No pattern provided. Replacing in all files.";
    }

    // Prepare flags for QDirIterator to include hidden subdirectories
    QDirIterator::IteratorFlags flags = m_searchOptions->includeSubdirectories
                                            ? QDirIterator::Subdirectories
                                            : QDirIterator::NoIteratorFlags;

    QDir::Filters dirFilters = QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot;

    // Process files in the directory
    QMimeDatabase mimeDb;
    QDirIterator it(m_searchOptions->location, dirFilters, flags);

    while (it.hasNext()) {
        QString filePath = it.next();
        QString fileName = QFileInfo(filePath).fileName();
        QMimeType mimeType = mimeDb.mimeTypeForFile(filePath);

        // Match MIME type and pattern (if provided)
        if (mimeType.name().startsWith("text/") &&
            (m_searchOptions->pattern.isEmpty() || regex.match(fileName).hasMatch())) {
            qInfo() << "Processing file for replace: " << filePath;
            replaceInFile(filePath);
        }
    }

    qInfo() << "Replace All completed. Total files processed: " << m_processedFiles;
}


void SystemReplaceDialog::countTextFiles(const QString& directory, bool includeSubdirectories, const QRegularExpression& pattern) {
    QMimeDatabase mimeDb;
    QDirIterator::IteratorFlags iteratorFlags = includeSubdirectories ?
                                                    QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags;
    QDirIterator it(directory, QDir::Files, iteratorFlags);

    while (it.hasNext()) {
        QString filePath = it.next();
        QString fileName = QFileInfo(filePath).fileName();
        QMimeType mimeType = mimeDb.mimeTypeForFile(filePath);

        // Match file name against the pattern
        if (mimeType.name().startsWith("text/") && pattern.match(fileName).hasMatch()) {
            m_files.insert(filePath);
        }
    }
}

void SystemReplaceDialog::processFile(const QString& filePath) {
    m_files.insert(filePath);
    qInfo() << "*** Included new file: " << filePath;

    m_processedFiles++;
    emit updateProgress(m_processedFiles, m_files.count());

    auto* worker = new FileSearchWorker(filePath, *m_searchOptions);
    connect(worker, &FileSearchWorker::fileProcessed, this, &SystemReplaceDialog::handleFileProcessed);
    m_threadPool.start(worker);
}

void SystemReplaceDialog::updateProgressDisplay(int processedFiles) {
    ui->m_progressBar->setValue(processedFiles);
    ui->m_statusLabel->setText(QString("Searching Files... %1/%2").arg(processedFiles).arg(m_files.count()));
}

void SystemReplaceDialog::handleFileProcessed(const FileSearchResults& result) {
    // Ensure the result dialog is open in non-modal mode
    if (!m_systemSearchResultDialog) {
        m_systemSearchResultDialog = new SystemSearchResultDialog(this);
        m_systemSearchResultDialog->setWindowModality(Qt::NonModal);
        m_systemSearchResultDialog->show();
    }

    // Add each result to the tree view in the dialog
    m_systemSearchResultDialog->addSearchResult(result);
}

void SystemReplaceDialog::UpdateSearchOptions() {
    if (!m_searchOptions) return;
    m_searchOptions->keyword = ui->comboBoxFind->currentText();
    m_searchOptions->replaceText = ui->comboBoxReplace->currentText();
    m_searchOptions->location = ui->comboBoxLocation->currentText();
    m_searchOptions->pattern = ui->comboBoxPattern->currentText();
    if (ui->replacePlainText->isChecked()) m_searchOptions->findMethod = FindMethod::SimpleText;
    if (ui->replaceRegularExpression->isChecked()){
        if (!Helpers::isValidRegularExpression(ui->comboBoxFind->currentText())) {
            QMessageBox::critical(this, "Error", "Invalid RegularExpression.");
        }
        m_searchOptions->findMethod = FindMethod::RegularExpression;
    }
    if (ui->replaceSpecialCharachters->isChecked()) {
        Helpers::notImplemented(this);
        m_searchOptions->findMethod = FindMethod::SpecialCharacters;
    }
    m_searchOptions->matchWholeWord = ui->matchWholeWord->isChecked();
    m_searchOptions->matchCase = ui->matchCase->isChecked();
    m_searchOptions->includeSubdirectories = ui->includeSubdirectories->isChecked();

    if (m_systemSearchResultDialog) {
        m_systemSearchResultDialog->setSearchOptions(*m_searchOptions);
    } else {
        qWarning() << "Cannot set replace options for replace result dialog.";
    }
}

void SystemReplaceDialog::showResultDialog() {
    if (!m_systemSearchResultDialog) {
        m_systemSearchResultDialog = new SystemSearchResultDialog(this);
        m_systemSearchResultDialog->setWindowModality(Qt::NonModal);

        connect(m_systemSearchResultDialog, &SystemSearchResultDialog::dialogClosed, this, [this]() {
            m_systemSearchResultDialog = nullptr;
        });

        m_systemSearchResultDialog->show();
    } else {
        m_systemSearchResultDialog->raise();
        m_systemSearchResultDialog->activateWindow();
    }

    UpdateSearchOptions();
}

void SystemReplaceDialog::on_findNext_clicked()
{
    m_searchOptions->role = Role::FindNext;
    UpdateSearchOptions();
    saveHistory();
    if (!m_systemSearchResultDialog) {
        startSearchNext(*m_searchOptions);
    } else {
        m_systemSearchResultDialog->traverseKeywords(false);
    }
    showResultDialog();
}

void SystemReplaceDialog::on_selectAll_clicked()
{
    m_files.clear();
    m_searchOptions->role = Role::SelectAll;
    UpdateSearchOptions();
    saveHistory();
    showResultDialog();
    selectAll(*m_searchOptions);
}

void SystemReplaceDialog::on_replaceAll_clicked()
{
    m_searchOptions->role = Role::ReplaceAll;
    UpdateSearchOptions();
    saveHistory();
    if (!m_systemSearchResultDialog) {
        startSearchNext(*m_searchOptions);
    } else {
        m_systemSearchResultDialog->replaceAllKeywords();
    }
    showResultDialog();
}

void SystemReplaceDialog::markUnsavedChanges() {
    m_unsavedChanges = true;
}

void SystemReplaceDialog::replaceInFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for reading: " << filePath;
        return;
    }

    QTextStream in(&file);
    QString fileContent = in.readAll();
    file.close();

    // Perform replacements in memory
    QString modifiedContent = fileContent;
    QRegularExpression searchRegex(
        QString("\\b%1\\b").arg(QRegularExpression::escape(m_searchOptions->keyword)),
        m_searchOptions->matchCase ? QRegularExpression::NoPatternOption
                                   : QRegularExpression::CaseInsensitiveOption
        );

    modifiedContent.replace(searchRegex, m_searchOptions->replaceText);

    if (modifiedContent != fileContent) {
        // Update the map of modified files
        m_modifiedFiles[filePath] = modifiedContent;
        markUnsavedChanges(); // Properly call the method here
        qInfo() << "Replaced content in memory for file: " << filePath;
    } else {
        qInfo() << "No changes made to file: " << filePath;
    }

    m_processedFiles++;
    ui->m_progressBar->setValue(m_processedFiles);
}


