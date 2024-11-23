#pragma once

#include <QDialog>
#include <QTextEdit>
#include "find.h"
#include "../codeeditor.h"
#include "../search/searchoptions.h"

namespace Ui {
class FindDialog;
}

class FindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindDialog(QWidget *parent = nullptr);
    ~FindDialog();

    static void showDialog(QWidget *parent = nullptr);
    QString getFindText() const;
    bool findPlainTextSelected() const;
    bool findRegularExpressionSelected() const;
    bool findSpecialCharachtersSelected() const;
    bool isWholeWordsChecked() const;
    bool isMatchCaseChecked() const;
    bool isAllTabsChecked() const;
    void setEditor(CodeEditor* editor);
    Find* getFind();
    SearchOptions* getSearchOptions();

signals:
    void findRequested(const QString& findText, bool matchCase, bool matchWholeWord, FindMethod mode);

private slots:

    void on_findPrevious_clicked();

    void on_findNext_clicked();

    void on_selectAll_clicked();

    void onAdvancedOptionsToggled(bool checked);

    void toggleAdvancedOptions(bool checked);

    void on_comboBoxFind_currentTextChanged(const QString &arg1);

    void on_comboBoxFind_currentIndexChanged(int index);

    void on_matchWholeWord_toggled(bool checked);

    void on_matchCase_toggled(bool checked);

    void on_checkBoxAllTabs_toggled(bool checked);

    void on_findRegularExpression_toggled(bool checked);

    void on_findPlainText_toggled(bool checked);

    void on_findSpecialCharachters_toggled(bool checked);

private:
    Ui::FindDialog *ui;
    FindMethod selectedFindMethod() const;
    SearchOptions* m_searchOptions;
    Find* m_find;
    CodeEditor* m_editor = nullptr;
    void populateComboBoxOnDropdown();
    void saveKeyword(const QString& keyword);
    bool eventFilter(QObject *watched, QEvent *event);
};

