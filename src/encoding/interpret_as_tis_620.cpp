#include "interpret_as_tis_620.h"
#include <QFile>
#include <QDebug>

// Singleton instance
Interpret_As_TIS_620& Interpret_As_TIS_620::instance() {
    static Interpret_As_TIS_620 instance;
    return instance;
}

// TIS-620 to Unicode mapping
uint32_t Interpret_As_TIS_620::tis620ToUnicode(uint8_t tis620Char) {
    if (tis620Char < 0x80) {
        return tis620Char; // ASCII range
    } else if (tis620Char >= 0xA1 && tis620Char <= 0xDA) {
        return 0x0E01 + (tis620Char - 0xA1); // Thai characters
    } else if (tis620Char >= 0xDF && tis620Char <= 0xFB) {
        return 0x0E3F + (tis620Char - 0xDF); // Thai punctuation and symbols
    }
    return 0xFFFD; // Replacement character for invalid mappings
}

// Decode the TIS-620 data
QString Interpret_As_TIS_620::decodeTIS620(const QByteArray& tis620Data) {
    QString result;

    for (char byte : tis620Data) {
        uint32_t unicodeChar = tis620ToUnicode(static_cast<uint8_t>(byte));
        result.append(QChar(unicodeChar));
    }

    return result;
}

// Execute the TIS-620 interpretation
void Interpret_As_TIS_620::execute(QPlainTextEdit* editor) {
    if (!editor) {
        qWarning() << "[ERROR] No editor instance provided.";
        return;
    }

    QString filePath = editor->property("filePath").toString();
    if (filePath.isEmpty()) {
        qWarning() << "[ERROR] No file path associated with the editor.";
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "[ERROR] Cannot open file:" << filePath;
        return;
    }

    QByteArray tis620Data = file.readAll();
    file.close();

    QString decodedText = decodeTIS620(tis620Data);
    editor->setPlainText(decodedText);
}

