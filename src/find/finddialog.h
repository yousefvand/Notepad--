#pragma once

#include <QDialog>
#include <QTextEdit>
#include "find.h"
#include "../codeeditor.h"

namespace Ui {
class FindDialog;
}

class FindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindDialog(QWidget *parent = nullptr);
    ~FindDialog();

    enum FindMode {
        PlainText,
        RegularExpression,
        SpecialCharacters
    };

    void performFind();
    static void showDialog(QWidget *parent = nullptr);
    QString getFindText() const;
    bool findPlainTextSelected() const;
    bool findRegularExpressionSelected() const;
    bool findSpecialCharachtersSelected() const;
    bool isWholeWordsChecked() const;
    bool isMatchCaseChecked() const;
    bool isAllTabsChecked() const;
    void setEditor(CodeEditor* editor);

signals:
    void findRequested(const QString& findText, bool matchCase, bool matchWholeWord, FindDialog::FindMode mode);

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
    FindMode selectedFindMode() const;
    SearchOptions* m_searchOptions;
    Find* m_find = nullptr;
    CodeEditor* m_editor = nullptr;
};

