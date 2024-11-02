#pragma once

#include <QDialog>

namespace Ui {
class IndentationDialog;
}

class IndentationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit IndentationDialog(QWidget *parent = nullptr);
    ~IndentationDialog();

    bool isTabsSelected() const;
    int getIndentationValue() const;
    void setTabsChecked(bool checked);
    bool isTabsChecked() const;

private slots:
    void onIndentationSelected();

    void on_buttonBox_accepted();

private:
    Ui::IndentationDialog *ui;
    void loadSettings();
    void saveSettings();
    void onOkClicked();
};

