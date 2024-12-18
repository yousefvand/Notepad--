#include "interpreteasdialog.h"
#include <QComboBox>
#include <QVBoxLayout>
#include <QDialogButtonBox>

InterpreteAsDialog::InterpreteAsDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Interpret As");

    // Create and populate the combobox
    comboBox = new QComboBox(this);
    comboBox->addItems({
        "UTF-8",
        "UTF-7",
        "UTF-32LE"
    });

    // Create OK and Cancel buttons using QDialogButtonBox
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    // Set layout for the dialog
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(comboBox);
    layout->addWidget(buttonBox);

    // Connect button signals to accept/reject the dialog
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

QString InterpreteAsDialog::getSelectedItem() const
{
    return comboBox->currentText();
}


