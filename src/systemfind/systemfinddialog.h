#pragma once

#include <QDebug>
#include <QDialog>
#include <QThreadPool>
#include <QCloseEvent>
#include "systemfind.h"
#include <atomic>
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

    void startSearchNext(const SearchOptions& options);
    void startSearchPrevious(const SearchOptions& options);
    void selectAll(const SearchOptions& options);

protected:
    void closeEvent(QCloseEvent *event) override {
        qWarning() << "Window is closing. Cleaning up resources...";
        cleanupResources();
        // Optionally accept or ignore the event
        event->accept(); // or event->ignore();
    }

signals:
    void updateProgress(int processedFiles, int totalFiles);

private slots:
    void handleFileProcessed(const FileSearchResults& result);

    void updateProgressDisplay(int processedFiles);

    void onAdvancedOptionsToggled(bool checked);

    void toggleAdvancedOptions(bool checked);

    void on_findNext_clicked();

    void on_pushButtonBrowse_clicked();

    void on_findPrevious_clicked();

    void on_selectAll_clicked();

private:
    Ui::SystemFindDialog *ui;

    void cleanupResources();
    FindMethod selectedFindMethod() const;
    SearchOptions* m_searchOptions;
    void populateComboBoxFind();
    void populateComboBoxLocation();
    void populateComboBoxPattern();
    void saveKeyword(const QString& keyword);
    void saveLocation(const QString& location);
    void savePattern(const QString& pattern);
    void saveHistory();
    bool eventFilter(QObject *watched, QEvent *event) override;
    void UpdateSearchOptions();
    void showResultDialog();

    void countTextFiles(const QString& directory, bool includeSubdirectories, const QRegularExpression& pattern);
    void processFile(const QString& filePath);
    QSet<QString> m_files;
    std::atomic<int> m_processedFiles{0};
    QThreadPool m_threadPool;
    SystemFind* m_find;
    SystemSearchResultDialog* m_systemSearchResultDialog;
};

