#pragma once

#include <QDialog>
#include <QStandardItemModel>
#include <QTreeView>
#include <QModelIndex>
#include "search/searchoptions.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SystemSearchResultDialog; }
QT_END_NAMESPACE

class SystemSearchResultDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SystemSearchResultDialog(QWidget *parent = nullptr);
    ~SystemSearchResultDialog();

    void addSearchResult(const FileSearchResults &result);
    void setSearchOptions(SearchOptions searchOptions);

signals:
    void openFileAtMatch(const QString &filePath, int lineNumber);

private slots:
    void handleDoubleClick(const QModelIndex &index);

private:
    int extractLineNumber(const QString &line) const;

    Ui::SystemSearchResultDialog *ui;
    QStandardItemModel* m_resultModel;
    SearchOptions m_searchOptions;
    QStandardItemModel* m_treeModel;
};
