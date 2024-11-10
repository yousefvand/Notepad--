#pragma once

#include <QDialog>
#include <QTextEdit>
#include "replace.h"
#include "../codeeditor.h"

namespace Ui {
class ReplaceDialog;
}

class ReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReplaceDialog(QWidget *parent = nullptr);
    ~ReplaceDialog();

    enum ReplaceMode {
        PlainText,
        RegularExpression,
        SpecialCharacters
    };

    void performReplace();
    static void showDialog(QWidget *parent = nullptr);
    QString getFindText() const;
    QString getReplaceText() const;
    bool replacePlainTextSelected() const;
    bool replaceRegularExpressionSelected() const;
    bool replaceSpecialCharachtersSelected() const;
    bool isWholeWordsChecked() const;
    bool isMatchCaseChecked() const;
    bool isAllTabsChecked() const;
    void setEditor(CodeEditor* editor);
    Replace* getReplace();
    SearchOptions* getSearchOptions();

signals:
    void replaceRequested(const QString& replaceText, bool matchCase, bool matchWholeWord, ReplaceDialog::ReplaceMode mode);

private slots:
    void toggleAdvancedOptions(bool checked);
    void onAdvancedOptionsToggled(bool checked);

    void on_findNext_clicked();

    void on_replaceNext_clicked();

    void on_findPrevious_clicked();

    void on_replacePrevious_clicked();

    void on_selectAll_clicked();

    void on_comboBoxFind_currentTextChanged(const QString &arg1);

    void on_comboBoxFind_currentIndexChanged(int index);

    void on_comboBoxReplace_currentTextChanged(const QString &arg1);

    void on_comboBoxReplace_currentIndexChanged(int index);

    void on_replacePlainText_toggled(bool checked);

    void on_replaceRegularExpression_toggled(bool checked);

    void on_replaceSpecialCharachters_toggled(bool checked);

    void on_matchWholeWord_toggled(bool checked);

    void on_matchCase_toggled(bool checked);

    void on_checkBoxAllTabs_toggled(bool checked);

    void on_replaceAll_clicked();

private:
    Ui::ReplaceDialog *ui;
    ReplaceMode selectedReplaceMode() const;
    SearchOptions* m_searchOptions;
    Replace* m_replace;
    CodeEditor* m_editor = nullptr;
    void saveKeyword(const QString& keyword);
    void saveReplaceWith(const QString& replaceWith);
    bool eventFilter(QObject *watched, QEvent *event);
    void populateComboBoxFind();
    void populateComboBoxReplace();
};

