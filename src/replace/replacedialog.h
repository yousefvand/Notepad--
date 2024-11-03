#pragma once

#include <QDialog>

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
    QString getReplaceText() const;
    bool isMatchCaseChecked() const;
    bool isWholeWordsChecked() const;

signals:
    void replaceRequested(const QString& replaceText, bool matchCase, bool matchWholeWord, ReplaceDialog::ReplaceMode mode);

private slots:
    void on_advancedOptions_checkStateChanged(const Qt::CheckState &arg1);

    void on_replacePrevious_clicked();

    void on_replaceNext_clicked();

    void on_selectAll_clicked();

    void handleReplaceRequest(const QString& replaceText, bool matchCase, bool matchWholeWord, ReplaceMode mode);

    void onAdvancedOptionsToggled(bool checked);

    void toggleAdvancedOptions(bool checked);

private:
    Ui::ReplaceDialog *ui;
    ReplaceMode selectedReplaceMode() const;
};

