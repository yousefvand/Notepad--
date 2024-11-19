#pragma once

#include <QDebug>
#include <QDialog>
#include <QThreadPool>
#include <QCloseEvent>
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

    void startSearchNext(const SearchOptions& options);
    void startSearchPrevious(const SearchOptions& options);

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

    void updateProgressDisplay(int processedFiles, int totalFiles);

    void onAdvancedOptionsToggled(bool checked);

    void toggleAdvancedOptions(bool checked);

    void on_findNext_clicked();

    void on_pushButtonBrowse_clicked();

    void on_findPrevious_clicked();

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
    bool eventFilter(QObject *watched, QEvent *event);
    void UpdateSearchOptions();
    void showResultDialog();

    void countTextFiles(const QString& directory, const QRegularExpression& pattern);
    void processFile(const QString& filePath);
    int m_totalFiles = 0;
    int m_processedFiles = 0;
    QThreadPool m_threadPool;
    SystemFind* m_find;
    SystemSearchResultDialog* m_systemSearchResultDialog;
};

