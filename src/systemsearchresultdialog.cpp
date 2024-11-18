#include "systemsearchresultdialog.h"
#include "ui_systemsearchresultdialog.h"
#include "helpers.h"
#include "systemtextdelegate.h"
#include <QRegularExpression>
#include <QStandardItem>
#include <QMessageBox>

SystemSearchResultDialog::SystemSearchResultDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SystemSearchResultDialog)
    , m_resultModel(new QStandardItemModel(this))
{
    ui->setupUi(this);
    // FIXME: treeview is not selectable and should be.

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
    header->setSectionsMovable(true);
    header->resizeSection(0, width() * 0.8); // Set initial size for 80% width for File Path
    header->resizeSection(1, width() * 0.2); // Set initial size for 20% width for Matches

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

void SystemSearchResultDialog::cleanupResources() {
    qInfo() << "Cleaning up resources. Closing window...";
}

void SystemSearchResultDialog::addSearchResult(const FileSearchResults &result) {
    QStandardItem *filePathItem = new QStandardItem(result.filePath);
    filePathItem->setFlags(filePathItem->flags() | Qt::ItemIsSelectable);

    int calculatedTotalMatchCount = 0;
    int firstKeywordLineNumber = -1; // Initialize to "not found"
    int currentLineNumber = 1;       // Start counting lines from 1

    for (const QString &line : result.matchingLines) {
        // Count occurrences of the keyword in the line
        int keywordCount = Helpers::countKeywordsInLine(line, m_searchOptions);

        if (keywordCount > 0) {
            calculatedTotalMatchCount += keywordCount;

            // Highlight keywords in the line
            QString highlightedLine = Helpers::highlightKeywords(line, m_searchOptions);

            // Track the first line containing a keyword
            if (firstKeywordLineNumber == -1) {
                firstKeywordLineNumber = currentLineNumber;
            }

            // Create tree view items for the line and keyword count
            QStandardItem *lineItem = new QStandardItem();
            lineItem->setFlags(lineItem->flags() | Qt::ItemIsSelectable);
            lineItem->setData(highlightedLine, Qt::DisplayRole);
            lineItem->setData(currentLineNumber, Qt::UserRole);

            QStandardItem *matchesItem = new QStandardItem(QString::number(keywordCount));
            matchesItem->setFlags(matchesItem->flags() | Qt::ItemIsSelectable);

            // Append the line and matches to the file path item
            filePathItem->appendRow({lineItem, matchesItem});
        }

        currentLineNumber++; // Increment the line counter
    }

    // Add total matches as a separate item
    QStandardItem *matchCountItem = new QStandardItem(QString::number(calculatedTotalMatchCount));
    matchCountItem->setFlags(matchCountItem->flags() | Qt::ItemIsSelectable);
    m_resultModel->appendRow({filePathItem, matchCountItem});

    // Emit signal for the first keyword line if found
    if (firstKeywordLineNumber != -1) {
        qDebug() << "First keyword line number calculated:" << firstKeywordLineNumber
                 << "for file:" << result.filePath;
        emit openFileAtMatch(result.filePath, firstKeywordLineNumber);
    } else {
        qDebug() << "No keywords found in file:" << result.filePath;
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

    if (lineNumber > 0) { // TODO: Remove MessageBox
        QMessageBox::information(this, "Double Click", QString("File: %1\nLine: %2").arg(filePath).arg(lineNumber));
        qDebug() << "Emitting openFileAtMatch signal for File Path:" << filePath << "Line Number:" << lineNumber;
        emit openFileAtMatch(filePath, lineNumber);
    } else {
        qWarning() << "Invalid line number detected for index:" << index;
    }
}

void SystemSearchResultDialog::setSearchOptions(SearchOptions searchOptions) {
    m_searchOptions = searchOptions;
}
