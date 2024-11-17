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

void SystemSearchResultDialog::addSearchResult(const FileSearchResults &result) {
    QStandardItem *filePathItem = new QStandardItem(result.filePath);
    int calculatedTotalMatchCount = 0;
    int lineNumber = 1;

    for (const QString &line : result.matchingLines) {
        int keywordCount = Helpers::countKeywordsInLine(line, m_searchOptions);
        if (keywordCount > 0) {
            calculatedTotalMatchCount += keywordCount;
            QString highlightedLine = Helpers::highlightKeywords(line, m_searchOptions);
            QStandardItem *lineItem = new QStandardItem();
            lineItem->setData(highlightedLine, Qt::DisplayRole);
            lineItem->setData(lineNumber, Qt::UserRole);

            QStandardItem *matchesItem = new QStandardItem(QString::number(keywordCount));
            filePathItem->appendRow({lineItem, matchesItem});
        }
        lineNumber++;
    }

    QStandardItem *matchCountItem = new QStandardItem(QString::number(calculatedTotalMatchCount));
    m_resultModel->appendRow({filePathItem, matchCountItem});

    qDebug() << "File:" << result.filePath
             << "Reported matches:" << result.matchCount
             << "Calculated matches (subtree sum):" << calculatedTotalMatchCount;
}

void SystemSearchResultDialog::handleDoubleClick(const QModelIndex &index) {
    if (!index.isValid()) return;

    // Debug: Log the clicked index
    qDebug() << "Double-click detected at index:" << index;

    // Check if it's a sub-item
    QModelIndex parentIndex = index.parent();
    if (!parentIndex.isValid()) {
        qDebug() << "Double-clicked on a top-level item.";
        return;
    }

    // Retrieve the file path from the parent item
    QString filePath = m_resultModel->itemFromIndex(parentIndex)->text();

    // Retrieve the line number from the clicked sub-item
    int lineNumber = index.data(Qt::UserRole).toInt();
    qDebug() << "Retrieved Line Number:" << lineNumber;

    if (lineNumber > 0) {
        //QMessageBox::information(this, "Double Click", QString("File: %1\nLine: %2").arg(filePath).arg(lineNumber));
        qDebug() << "Emitting openFileAtMatch signal with File Path:" << filePath << "Line Number:" << lineNumber;
        emit openFileAtMatch(filePath, lineNumber);
    } else {
        qWarning() << "Line number conversion failed for index:" << index;
    }
}

void SystemSearchResultDialog::setSearchOptions(SearchOptions searchOptions) {
    m_searchOptions = searchOptions;
}
