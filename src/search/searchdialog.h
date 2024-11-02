#pragma once

#include <QDialog>

namespace Ui {
class SearchDialog;
}

class SearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SearchDialog(QWidget *parent = nullptr);
    ~SearchDialog();

    enum SearchMode {
        PlainText,
        RegularExpression,
        SpecialCharacters
    };

    void performSearch();
    static void showDialog(QWidget *parent = nullptr);
    QString getSearchText() const;
    bool isMatchCaseChecked() const;
    bool isWholeWordsChecked() const;

signals:
    void searchRequested(const QString& searchText, bool matchCase, bool matchWholeWord, SearchDialog::SearchMode mode);

private slots:
    void on_advancedOptions_checkStateChanged(const Qt::CheckState &arg1);

    void on_findPrevious_clicked();

    void on_findNext_clicked();

    void on_selectAll_clicked();

    void handleSearchRequest(const QString& searchText, bool matchCase, bool matchWholeWord, SearchMode mode);

    void onAdvancedOptionsToggled(bool checked);

    void toggleAdvancedOptions(bool checked);

private:
    Ui::SearchDialog *ui;
    SearchMode selectedSearchMode() const;
};

