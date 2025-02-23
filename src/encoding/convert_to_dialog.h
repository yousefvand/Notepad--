#pragma once

#include <QDialog>
#include <QString>

class QComboBox;

class ConvertToDialog : public QDialog
{
public:
    explicit ConvertToDialog(QWidget* parent = nullptr);
    QString getSelectedItem() const;

private:
    QComboBox* comboBox; // Pointer to the combobox
};
