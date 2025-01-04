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
        "TSCII",
        "TIS-620",
        "SHIFT-JTS",
        "SCSU",
        "KOI8-U",
        "KOI8-R",
        "ISO-8859-15",
        "ISO-8859-14",
        "ISO-8859-13",
        "ISO-8859-10",
        "ISO-8859-9",
        "ISO-8859-8",
        "ISO-8859-7",
        "ISO-8859-6",
        "ISO-8859-5",
        "ISO-8859-4",
        "ISO-8859-3",
        "ISO-8859-2",
        "ISO-8859-1",
        "ISO-2022-KR",
        "ISO-2022-JP-2",
        "ISO-2022-JP-1",
        "ISO-2022-JP",
        "ISO-2022-CN-EXT",
        "ISO-2022-CN",
        "IBM-918",
        "IBM-871",
        "IBM-870"
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


