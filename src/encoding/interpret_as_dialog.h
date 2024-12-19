#pragma once

#include <QDialog>
#include <QString>

class QComboBox;

class InterpreteAsDialog : public QDialog
{
public:
    explicit InterpreteAsDialog(QWidget* parent = nullptr);
    QString getSelectedItem() const;

private:
    QComboBox* comboBox; // Pointer to the combobox
};
