#pragma once

#include <QDialog>

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
    bool isMatchCaseChecked() const;
    bool isWholeWordsChecked() const;

signals:
    void findRequested(const QString& findText, bool matchCase, bool matchWholeWord, FindDialog::FindMode mode);

private slots:
    void on_advancedOptions_checkStateChanged(const Qt::CheckState &arg1);

    void on_findPrevious_clicked();

    void on_findNext_clicked();

    void on_selectAll_clicked();

    void handleFindRequest(const QString& findText, bool matchCase, bool matchWholeWord, FindMode mode);

    void onAdvancedOptionsToggled(bool checked);

    void toggleAdvancedOptions(bool checked);

private:
    Ui::FindDialog *ui;
    FindMode selectedFindMode() const;
};

