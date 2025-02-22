#pragma once

#include <QDebug>
#include <QDialog>
#include <QStandardItemModel>
#include <QTreeView>
#include <QModelIndex>
#include <QCloseEvent>
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
    void traverseKeywords(bool backward);
    void traverseAndReplaceKeywords(bool backward);
    void replaceAllKeywords();

    void markUnsavedChanges();
    bool saveChanges();
    bool hasUnsavedChanges() const;

protected:
    void closeEvent(QCloseEvent *event) override;

signals:
    void dialogClosed();
    void openFileAtMatch(const QString &filePath, int lineNumber);

private slots:
    void handleDoubleClick(const QModelIndex &index);

private:
    int extractLineNumber(const QString &line) const;

    Ui::SystemSearchResultDialog *ui;
    bool m_unsavedChanges;
    QMap<QString, QString> m_modifiedFiles;
    void cleanupResources();
    QStandardItemModel* m_resultModel;
    SearchOptions m_searchOptions;
    QStandardItemModel* m_treeModel;
    QString removeHtmlTags(const QString& text);
};
