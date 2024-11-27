#pragma once
#include <QDialog>
#include <QThreadPool>
#include "systemreplace.h"
#include "../search/searchoptions.h"
#include "../systemsearchresultdialog.h"

namespace Ui {
class SystemReplaceDialog;
}

class SystemReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SystemReplaceDialog(QWidget *parent = nullptr);
    ~SystemReplaceDialog();

    void startSearchNext(const SearchOptions& options);
    void startReplaceNext(const SearchOptions& options);
    void startSearchPrevious(const SearchOptions& options);
    void startReplacePrevious(const SearchOptions& options);
    void selectAll(const SearchOptions& options);
    void replaceAll(const SearchOptions& options);
    void markUnsavedChanges();

protected:
    QMap<QString, QString> m_modifiedFiles;
    bool m_unsavedChanges;

signals:
    void updateProgress(int processedFiles, int totalFiles);

private slots:
    void handleFileProcessed(const FileSearchResults& result);

    void onAdvancedOptionsToggled(bool checked);

    void toggleAdvancedOptions(bool checked);

    void updateProgressDisplay(int processedFiles);

    void on_findNext_clicked();

    void on_pushButtonBrowse_clicked();

    void on_findPrevious_clicked();

    void on_selectAll_clicked();

    void on_replaceNext_clicked();

    void on_replacePrevious_clicked();

    void on_replaceAll_clicked();

private:
    Ui::SystemReplaceDialog *ui;

    void cleanupResources();
    FindMethod selectedFindMode() const;
    SearchOptions* m_searchOptions;
    SystemReplace* m_replace;
    bool eventFilter(QObject *watched, QEvent *event);
    void saveKeyword(const QString& keyword);
    void saveReplaceWith(const QString& replaceWith);
    void saveLocation(const QString& location);
    void savePattern(const QString& pattern);
    void populateComboBoxFind();
    void populateComboBoxReplace();
    void populateComboBoxLocation();
    void populateComboBoxPattern();

    void countTextFiles(const QString& directory, bool includeSubdirectories, const QRegularExpression& pattern);
    void UpdateSearchOptions();
    void processFile(const QString& filePath);
    void replaceInFile(const QString& filePath);
    void showResultDialog();
    void saveHistory();
    QSet<QString> m_files;
    std::atomic<int> m_processedFiles{0};
    QThreadPool m_threadPool;
    SystemReplace* m_find;
    SystemSearchResultDialog* m_systemSearchResultDialog;
};

