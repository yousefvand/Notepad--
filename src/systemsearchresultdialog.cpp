#include "systemsearchresultdialog.h"
#include "ui_systemsearchresultdialog.h"
#include "systemtextdelegate.h"
#include <QRegularExpression>
#include <QStandardItem>
#include <QMessageBox>
#include <QFile>

SystemSearchResultDialog::SystemSearchResultDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SystemSearchResultDialog)
    , m_unsavedChanges(false)
    , m_resultModel(new QStandardItemModel(this))
{
    ui->setupUi(this);

    m_treeModel = new QStandardItemModel(this);
    ui->resultTreeView->setModel(m_treeModel);

    // Set up the model for the tree view
    m_resultModel->setHorizontalHeaderLabels({"File Path", "Matches"});
    ui->resultTreeView->setModel(m_resultModel);

    // Enable interactive resizing and allow moving columns
    ui->resultTreeView->header()->setSectionResizeMode(QHeaderView::Interactive); // Enable user resizing
    ui->resultTreeView->header()->setStretchLastSection(false);                  // Allow resizing of all columns

    // Adjust column widths and make them proportional
    QHeaderView *header = ui->resultTreeView->header();
    header->setSectionResizeMode(0, QHeaderView::Stretch); // File Path column stretches
    header->setSectionResizeMode(1, QHeaderView::Interactive); // Matches column fixed but adjustable
    header->setStretchLastSection(false);
    header->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    header->setSectionsMovable(true);
    header->resizeSection(0, width() * 0.9); // Set initial size for 90% width for File Path
    header->resizeSection(1, width() * 0.1); // Set initial size for 10% width for Matches

    ui->resultTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->resultTreeView->setSelectionMode(QAbstractItemView::SingleSelection);

    // Prevent editing and set double-click expansion
    ui->resultTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->resultTreeView->setItemsExpandable(true);
    ui->resultTreeView->setExpandsOnDoubleClick(true);
    ui->resultTreeView->setTextElideMode(Qt::ElideNone);

    // Apply the custom text delegate
    ui->resultTreeView->setItemDelegate(new SystemTextDelegate(this));
    ui->resultTreeView->setUniformRowHeights(false); // Allow rows to have variable heights
    ui->resultTreeView->resizeColumnToContents(0);  // Ensure columns fit the content
    ui->resultTreeView->doItemsLayout();

    connect(ui->resultTreeView, &QTreeView::doubleClicked, this, &SystemSearchResultDialog::handleDoubleClick);
}

SystemSearchResultDialog::~SystemSearchResultDialog()
{
    delete ui;
}

void SystemSearchResultDialog::markUnsavedChanges() {
    m_unsavedChanges = true;
}

bool SystemSearchResultDialog::hasUnsavedChanges() const {
    return m_unsavedChanges;
}

bool SystemSearchResultDialog::saveChanges() {
    for (auto it = m_modifiedFiles.begin(); it != m_modifiedFiles.end(); ++it) {
        QFile file(it.key());
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::critical(this, tr("Error"), tr("Failed to save file: %1").arg(it.key()));
            return false; // Abort saving if a file fails
        }

        QTextStream out(&file);
        out << it.value(); // Write the modified content
        file.close();
    }

    m_unsavedChanges = false;
    m_modifiedFiles.clear(); // Clear the tracked changes after saving
    return true;
}

void SystemSearchResultDialog::closeEvent(QCloseEvent *event) {
    if (hasUnsavedChanges()) {
        QMessageBox::StandardButton response = QMessageBox::warning(
            this,
            tr("Unsaved Changes"),
            tr("You have unsaved changes. Do you want to save them?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
            QMessageBox::Cancel
            );

        switch (response) {
        case QMessageBox::Save:
            if (saveChanges()) {
                emit dialogClosed();
                QDialog::closeEvent(event);
                event->accept(); // Allow closing
            } else {
                event->ignore(); // Cancel closing if saving fails
            }
            break;
        case QMessageBox::Discard:
            emit dialogClosed();
            QDialog::closeEvent(event);
            event->accept(); // Allow closing
            break;
        case QMessageBox::Cancel:
        default:
            event->ignore(); // Cancel closing
            break;
        }
    } else {
        emit dialogClosed();
        QDialog::closeEvent(event);
        event->accept(); // No unsaved changes, allow closing
    }
}

// TODO: Pass SearchOptions to this function so it can highlight all keywords.
void SystemSearchResultDialog::addSearchResult(const FileSearchResults &result) {
    if (result.matches.isEmpty()) {
        qWarning() << "No matches found for file:" << result.filePath;
        return;
    }

    QStandardItem *filePathItem = new QStandardItem(result.filePath);
    filePathItem->setFlags(filePathItem->flags() | Qt::ItemIsSelectable);

    QStandardItem *matchCountItem = new QStandardItem(QString::number(result.matchCount));
    matchCountItem->setFlags(matchCountItem->flags() | Qt::ItemIsSelectable);

    int firstKeywordLineNumber = -1; // To track the first match line number

    // Iterate through the matches to populate the result model
    for (const auto &[lineNumber, lineContent] : result.matches) {
        // Highlight keywords with cyan
        QString highlightedLine = lineContent;
        QString pattern = QStringLiteral("<span style='background-color: yellow;'>%1</span>");
        highlightedLine.replace(
            QRegularExpression(QString("\\b%1\\b").arg(QRegularExpression::escape(m_searchOptions.keyword)),
                               m_searchOptions.matchCase
                                   ? QRegularExpression::NoPatternOption
                                   : QRegularExpression::CaseInsensitiveOption),
            pattern.arg(m_searchOptions.keyword));

        QStandardItem *lineItem = new QStandardItem();
        lineItem->setFlags(lineItem->flags() | Qt::ItemIsSelectable);
        lineItem->setData(highlightedLine, Qt::DisplayRole); // Highlighted line content
        lineItem->setData(lineNumber + 1, Qt::UserRole); // Line number (1-based)

        QStandardItem *matchesItem = new QStandardItem(QString::number(1));
        matchesItem->setFlags(matchesItem->flags() | Qt::ItemIsSelectable);

        filePathItem->appendRow({lineItem, matchesItem});

        // Store the first keyword's line number (1-based)
        if (firstKeywordLineNumber == -1) {
            firstKeywordLineNumber = lineNumber + 1; // Convert to 1-based
        }
    }

    // Add the file path and match count as a top-level item
    m_resultModel->appendRow({filePathItem, matchCountItem});

    qDebug() << "File:" << result.filePath
             << "Reported matches:" << result.matchCount
             << "First keyword line number:" << firstKeywordLineNumber;

    // If needed, handle the first keyword line highlighting
    if (firstKeywordLineNumber > 0) {
        qDebug() << "First keyword found at line:" << firstKeywordLineNumber << "in file:" << result.filePath;
        // Call the highlighting function here, if necessary
        // Example: highlightLineInView(result.filePath, firstKeywordLineNumber);
    }
}

// FIXME: Line number is wrong
void SystemSearchResultDialog::handleDoubleClick(const QModelIndex &index) {
    if (!index.isValid()) {
        qWarning() << "Invalid index detected in handleDoubleClick.";
        return;
    }

    // Debug: Log the clicked index details
    qDebug() << "Double-click detected at index:" << index << "Data:" << index.data(Qt::DisplayRole);

    // Check if it's a sub-item by verifying its parent index
    QModelIndex parentIndex = index.parent();
    if (!parentIndex.isValid()) {
        qDebug() << "Double-clicked on a top-level item (file path), ignoring.";
        return;
    }

    // Retrieve the file path from the parent item
    QStandardItem *parentItem = m_resultModel->itemFromIndex(parentIndex);
    if (!parentItem) {
        qWarning() << "Failed to retrieve parent item for index:" << index;
        return;
    }
    QString filePath = parentItem->text();

    // Retrieve the line number from the clicked sub-item
    QVariant lineData = index.data(Qt::UserRole);
    if (!lineData.isValid()) {
        qWarning() << "No valid line number data for index:" << index;
        return;
    }

    int lineNumber = lineData.toInt();
    qDebug() << "File Path:" << filePath << "Retrieved Line Number:" << lineNumber;

    if (lineNumber > 0) {
        qDebug() << "Emitting openFileAtMatch signal for File Path:" << filePath << "Line Number:" << lineNumber;
        emit openFileAtMatch(filePath, lineNumber);
    } else {
        qWarning() << "Invalid line number detected for index:" << index;
    }
}

void SystemSearchResultDialog::setSearchOptions(SearchOptions searchOptions) {
    m_searchOptions = searchOptions;
}

void SystemSearchResultDialog::traverseKeywords(bool backward) {
    static int currentKeywordIndex = -1; // Tracks the current keyword index
    static QModelIndex previousItemIndex; // Tracks the previously highlighted index

    // Reset the highlight of the previous item if it exists
    if (previousItemIndex.isValid()) {
        QStandardItem* previousItem = m_resultModel->itemFromIndex(previousItemIndex);
        if (previousItem) {
            QString originalText = previousItem->data(Qt::UserRole + 1).toString();
            if (!originalText.isEmpty()) {
                previousItem->setData(originalText, Qt::EditRole); // Restore original text
            }
        }
    }

    // Count total matches to handle wraparound
    int totalMatches = 0;
    for (int row = 0; row < m_resultModel->rowCount(); ++row) {
        QStandardItem* fileItem = m_resultModel->item(row, 0);
        if (!fileItem) continue;
        totalMatches += fileItem->rowCount();
    }

    // Adjust the keyword index based on the traversal direction
    if (backward) {
        currentKeywordIndex--;
        if (currentKeywordIndex < 0) {
            currentKeywordIndex = totalMatches - 1; // Wrap around to the last match
        }
    } else {
        currentKeywordIndex++;
        if (currentKeywordIndex >= totalMatches) {
            currentKeywordIndex = 0; // Wrap around to the first match
        }
    }

    // Find and highlight the corresponding match
    int matchCounter = 0;
    QModelIndex nextItemIndex;
    for (int row = 0; row < m_resultModel->rowCount(); ++row) {
        QStandardItem* fileItem = m_resultModel->item(row, 0);
        if (!fileItem) continue;

        for (int subRow = 0; subRow < fileItem->rowCount(); ++subRow) {
            QStandardItem* lineItem = fileItem->child(subRow, 0);
            if (!lineItem) continue;

            if (matchCounter == currentKeywordIndex) {
                nextItemIndex = m_resultModel->indexFromItem(lineItem);

                // Highlight the keyword with a cyan background
                QString lineText = lineItem->data(Qt::EditRole).toString();
                QString originalText = lineItem->data(Qt::UserRole + 1).toString();
                if (originalText.isEmpty()) {
                    originalText = lineText;
                    lineItem->setData(originalText, Qt::UserRole + 1); // Store original text
                }

                QString highlightedLine = originalText;
                highlightedLine.replace(
                    QRegularExpression(QString("\\b%1\\b").arg(QRegularExpression::escape(m_searchOptions.keyword))),
                    QString("<span style='background-color: cyan; color: black;'>%1</span>").arg(m_searchOptions.keyword));

                lineItem->setData(highlightedLine, Qt::EditRole);

                // Scroll to and select the highlighted item
                ui->resultTreeView->scrollTo(nextItemIndex);
                ui->resultTreeView->setCurrentIndex(nextItemIndex);

                previousItemIndex = nextItemIndex; // Save the current item for the next call
                return;
            }

            matchCounter++;
        }
    }
}

QString SystemSearchResultDialog::removeHtmlTags(const QString& text) {
    static const QRegularExpression htmlTagRegex("<[^>]*>");
    QString cleanedText = text;
    cleanedText.remove(htmlTagRegex);
    return cleanedText;
}

void SystemSearchResultDialog::traverseAndReplaceKeywords(bool backward) {
    static int currentKeywordIndex = -1; // Tracks the current keyword index
    static QModelIndex previousItemIndex; // Tracks the previously highlighted index

    // Calculate total matches
    int totalMatches = 0;
    for (int row = 0; row < m_resultModel->rowCount(); ++row) {
        QStandardItem* fileItem = m_resultModel->item(row, 0);
        if (!fileItem) continue;
        totalMatches += fileItem->rowCount(); // Count sub-items (lines)
    }

    // Adjust the index for forward or backward traversal
    if (backward) {
        currentKeywordIndex--;
        if (currentKeywordIndex < 0) currentKeywordIndex = totalMatches - 1; // Wrap to the last match
    } else {
        currentKeywordIndex++;
        if (currentKeywordIndex >= totalMatches) currentKeywordIndex = 0; // Wrap to the first match
    }

    qDebug() << "Total matches:" << totalMatches;
    qDebug() << "Current keyword index:" << currentKeywordIndex;

    // Traverse matches
    int matchCounter = 0;
    QModelIndex nextItemIndex;
    for (int row = 0; row < m_resultModel->rowCount(); ++row) {
        QStandardItem* fileItem = m_resultModel->item(row, 0);
        if (!fileItem) continue;

        QString filePath = fileItem->text();
        QString fileContent = m_modifiedFiles.value(filePath);

        for (int subRow = 0; subRow < fileItem->rowCount(); ++subRow) {
            QStandardItem* lineItem = fileItem->child(subRow, 0);
            if (!lineItem) continue;

            if (matchCounter == currentKeywordIndex) {
                nextItemIndex = m_resultModel->indexFromItem(lineItem);

                // Retrieve the line content and line number
                QString lineText = lineItem->data(Qt::EditRole).toString();
                int lineNumber = lineItem->data(Qt::UserRole).toInt() - 1; // Convert to 0-based index

                qDebug() << "Processing line:" << lineText << "at line number:" << lineNumber;

                // Remove <highlight> tags from the line
                QString cleanLine = lineText;
                static QRegularExpression htmlTagRegex("<[^>]*>");
                cleanLine.remove(htmlTagRegex);
                qDebug() << "Cleaned line text:" << cleanLine;

                // Replace keyword in the cleaned line content
                QRegularExpression regex(
                    QString("\\b%1\\b").arg(QRegularExpression::escape(m_searchOptions.keyword)),
                    m_searchOptions.matchCase ? QRegularExpression::NoPatternOption
                                              : QRegularExpression::CaseInsensitiveOption
                    );

                QString replacedText = cleanLine;
                replacedText.replace(regex, m_searchOptions.replaceText);
                qDebug() << "Replaced line text:" << replacedText;

                if (replacedText != cleanLine) {
                    // Update tree view
                    lineItem->setData(replacedText, Qt::EditRole);
                    qDebug() << "Updated line in tree view to:" << replacedText;

                    // Update in-memory file content
                    QStringList fileLines = fileContent.split('\n');
                    if (lineNumber >= 0 && lineNumber < fileLines.size()) {
                        fileLines[lineNumber] = replacedText;
                        m_modifiedFiles[filePath] = fileLines.join('\n');
                        qDebug() << "Updated file content in memory for:" << filePath;
                    }

                    markUnsavedChanges();
                } else {
                    qDebug() << "No replacement made for line:" << cleanLine;
                }

                // Scroll to and select the item
                ui->resultTreeView->scrollTo(nextItemIndex);
                ui->resultTreeView->setCurrentIndex(nextItemIndex);

                previousItemIndex = nextItemIndex; // Save current item index
                return;
            }

            matchCounter++;
        }
    }

    qDebug() << "Traversal completed. No match found for current index:" << currentKeywordIndex;
}

void SystemSearchResultDialog::replaceAllKeywords() {
    qDebug() << "Starting replace all keywords";

    // Iterate over all rows in the result model
    for (int row = 0; row < m_resultModel->rowCount(); ++row) {
        QStandardItem* fileItem = m_resultModel->item(row, 0);
        if (!fileItem) continue;

        QString filePath = fileItem->text();
        QString fileContent = m_modifiedFiles.value(filePath);
        QStringList fileLines = fileContent.split('\n');
        bool fileModified = false;

        // Iterate over sub-items (lines) in each file
        for (int subRow = 0; subRow < fileItem->rowCount(); ++subRow) {
            QStandardItem* lineItem = fileItem->child(subRow, 0);
            if (!lineItem) continue;

            // Retrieve the line content and line number
            QString lineText = lineItem->data(Qt::EditRole).toString();
            int lineNumber = lineItem->data(Qt::UserRole).toInt() - 1; // Convert to 0-based index

            qDebug() << "Processing line:" << lineText << "at line number:" << lineNumber;

            // Remove <highlight> tags from the line
            QString cleanLine = lineText;
            static QRegularExpression htmlTagRegex("<[^>]*>");
            cleanLine.remove(htmlTagRegex);
            qDebug() << "Cleaned line text:" << cleanLine;

            // Replace keyword in the cleaned line content
            QRegularExpression regex(
                QString("\\b%1\\b").arg(QRegularExpression::escape(m_searchOptions.keyword)),
                m_searchOptions.matchCase ? QRegularExpression::NoPatternOption
                                          : QRegularExpression::CaseInsensitiveOption
                );

            QString replacedText = cleanLine;
            replacedText.replace(regex, m_searchOptions.replaceText);
            qDebug() << "Replaced line text:" << replacedText;

            if (replacedText != cleanLine) {
                // Update tree view
                lineItem->setData(replacedText, Qt::EditRole);
                qDebug() << "Updated line in tree view to:" << replacedText;

                // Update in-memory file content
                if (lineNumber >= 0 && lineNumber < fileLines.size()) {
                    fileLines[lineNumber] = replacedText;
                    fileModified = true;
                }
            } else {
                qDebug() << "No replacement made for line:" << cleanLine;
            }
        }

        // If the file was modified, update m_modifiedFiles
        if (fileModified) {
            m_modifiedFiles[filePath] = fileLines.join('\n');
            qDebug() << "Updated file content in memory for:" << filePath;
        }
    }

    markUnsavedChanges(); // Ensure unsaved changes are marked
    qDebug() << "Replace all keywords completed.";
}
