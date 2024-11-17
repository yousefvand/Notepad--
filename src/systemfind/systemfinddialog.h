#pragma once

#include <QDialog>
#include <QThreadPool>
#include "systemfind.h"
#include "../search/searchoptions.h"
#include "../systemsearchresultdialog.h"

namespace Ui {
class SystemFindDialog;
}

class QProgressBar;
class QLabel;

class SystemFindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SystemFindDialog(QWidget *parent = nullptr);
    ~SystemFindDialog();

    void startSearch(const SearchOptions& options);

signals:
    void updateProgress(int processedFiles, int totalFiles);

private slots:
    void handleFileProcessed(const FileSearchResults& result);

    void updateProgressDisplay(int processedFiles, int totalFiles);

    void onAdvancedOptionsToggled(bool checked);

    void toggleAdvancedOptions(bool checked);

    void on_findNext_clicked();

    void on_pushButtonBrowse_clicked();

private:
    Ui::SystemFindDialog *ui;

    FindMethod selectedFindMethod() const;
    SearchOptions* m_searchOptions;
    void populateComboBoxFind();
    void populateComboBoxLocation();
    void populateComboBoxPattern();
    void saveKeyword(const QString& keyword);
    void saveLocation(const QString& location);
    void savePattern(const QString& pattern);
    bool eventFilter(QObject *watched, QEvent *event);
    void UpdateSearchOptions();
    void showResultDialog();

    void countTextFiles(const QString& directory);
    void processFile(const QString& filePath);
    int m_totalFiles = 0;
    int m_processedFiles = 0;
    QThreadPool m_threadPool;
    SystemFind* m_find;
    SystemSearchResultDialog* m_systemSearchResultDialog;
};

