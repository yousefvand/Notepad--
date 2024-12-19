#include "interpret_as_dialog.h"
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
        "UTF-32LE",
        "UTF-32BE",
        "UTF-32",
        "UTF-16LE",
        "UTF16BE",
        "UTF-16",
        "US-ASCII",
        "TSCII"
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


