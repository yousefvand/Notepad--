#include "convert_to_utf_7.h"
#include "../codeeditor.h"

#include <QByteArray>
#include <QStringEncoder>

Convert_To_Utf_7& Convert_To_Utf_7::instance() {
    static Convert_To_Utf_7 instance;
    return instance;
}

void Convert_To_Utf_7::execute(CodeEditor* editor) {
    if (!editor) return;

    QString text = editor->toPlainText();  // Get text from editor
    QString utf7Text = encodeToUtf7(text); // Convert to UTF-7

    editor->setPlainText(utf7Text);  // Update editor content
}

QString Convert_To_Utf_7::encodeToUtf7(const QString& text) {
    // Convert QString to UTF-16 using QStringEncoder
    QStringEncoder encoder(QStringConverter::Utf16);
    QByteArray utf16Data = encoder.encode(text);

    if (utf16Data.isEmpty()) {
        qDebug() << "Error: UTF-16 encoding failed!";
        return QString();
    }

    QString utf7String;
    bool inBase64 = false;

    for (int i = 0; i < utf16Data.size() - 1; i += 2) {  // Ensure valid UTF-16 pairs
        ushort unicodeValue = static_cast<uchar>(utf16Data[i]) | (static_cast<uchar>(utf16Data[i + 1]) << 8);

        if (unicodeValue > 0xFFFF) {  // Handle surrogate pairs properly
            qDebug() << "Skipping invalid UTF-16 character: " << unicodeValue;
            continue;
        }

        QChar character(unicodeValue);

        if (character.unicode() >= 0x20 && character.unicode() <= 0x7E && character != '+') {
            if (inBase64) {
                utf7String.append('-');  // End Base64 encoding
                inBase64 = false;
            }
            utf7String.append(character);
        } else {
            if (!inBase64) {
                utf7String.append('+');  // Start Base64 sequence
                inBase64 = true;
            }
            QByteArray base64Chunk = QByteArray::fromRawData(reinterpret_cast<const char*>(&unicodeValue), sizeof(ushort))
                                         .toBase64(QByteArray::Base64Encoding | QByteArray::OmitTrailingEquals);
            utf7String.append(QString::fromUtf8(base64Chunk));
        }
    }

    if (inBase64) {
        utf7String.append('-');  // Close Base64 sequence if open
    }

    return utf7String;
}
