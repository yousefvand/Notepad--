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

void SystemSearchResultDialog::closeEvent(QCloseEvent* event) {
    emit dialogClosed();
    QDialog::closeEvent(event);
    event->accept();
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

    if (lineNumber > 0) { // TODO: Remove MessageBox
        //QMessageBox::information(this, "Double Click", QString("File: %1\nLine: %2").arg(filePath).arg(lineNumber));
        qDebug() << "Emitting openFileAtMatch signal for File Path:" << filePath << "Line Number:" << lineNumber;
        emit openFileAtMatch(filePath, lineNumber);
    } else {
        qWarning() << "Invalid line number detected for index:" << index;
    }
}

void SystemSearchResultDialog::setSearchOptions(SearchOptions searchOptions) {
    m_searchOptions = searchOptions;
}


