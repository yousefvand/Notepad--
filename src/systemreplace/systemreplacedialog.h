#pragma once
#include <QDialog>
#include "systemreplace.h"
#include "../search/searchoptions.h"

namespace Ui {
class SystemReplaceDialog;
}

class SystemReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SystemReplaceDialog(QWidget *parent = nullptr);
    ~SystemReplaceDialog();

private slots:
    void onAdvancedOptionsToggled(bool checked);

    void toggleAdvancedOptions(bool checked);

    void on_findNext_clicked();

private:
    Ui::SystemReplaceDialog *ui;

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
};

