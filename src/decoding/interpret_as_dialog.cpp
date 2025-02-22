#include "interpret_as_dialog.h"
#include <QComboBox>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <qabstractitemview.h>

InterpreteAsDialog::InterpreteAsDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Interpret As");

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
        "IBM-870",
        "IBM-869",
        "IBM-868",
        "IBM-866",
        "IBM-865",
        "IBM-864",
        "IBM-863",
        "IBM-862",
        "IBM-861",
        "IBM-860",
        "IBM-857",
        "IBM-855",
        "IBM-852",
        "IBM-850",
        "IBM-775",
        "IBM-500",
        "IBM-437",
        "IBM-424",
        "IBM-420",
        "IBM-297",
        "IBM-290",
        "IBM-285",
        "IBM-284",
        "IBM-278",
        "IBM-277",
        "IBM-273",
        "IBM-1047",
        "IBM-1026",
        "IBM-037",
        "IBM-01149",
        "IBM-01148",
        "IBM-01147",
        "IBM-01146",
        "IBM-01145",
        "IBM-01144",
        "IBM-01143",
        "IBM-01142",
        "IBM-01141",
        "IBM-01140",
        "IBM-00858",
        "IBM-Thai",
        "HZ-GB-2312",
        "GBK",
        "GB18030",
        "EUC-JP",
        "CESU-8",
        "Big5-HKSCS",
        "Big5",
        "BOCU-1",
        "Adobe-Standard-Encoding",
        "cp851",
        "hp-roman8",
        "macintosh",
        "windows-1250",
        "windows-1251",
        "windows-1252",
        "windows-1253",
        "windows-1254",
        "windows-1255",
        "windows-1256",
        "windows-1257",
        "windows-1258",
        "windows-949"
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


