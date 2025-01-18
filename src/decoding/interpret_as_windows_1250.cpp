#include "../codeeditor.h"
#include "interpret_as_windows_1250.h"
#include <QFile>
#include <QDebug>

// Singleton instance
Interpret_As_windows_1250& Interpret_As_windows_1250::instance() {
    static Interpret_As_windows_1250 instance;
    return instance;
}

// Constructor
Interpret_As_windows_1250::Interpret_As_windows_1250() {}

// Main execution function
void Interpret_As_windows_1250::execute(QPlainTextEdit* editor) {
    if (!editor) {
        qWarning() << "[ERROR] No editor instance provided.";
        return;
    }

    CodeEditor* codeEditor = qobject_cast<CodeEditor*>(editor);
    if (!codeEditor) {
        qWarning() << "[ERROR] Invalid CodeEditor instance.";
        return;
    }

    QString filePath = codeEditor->filePath();
    if (filePath.isEmpty()) {
        qWarning() << "[ERROR] No file path associated with the editor.";
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "[ERROR] Cannot open file:" << filePath;
        return;
    }

    QByteArray rawData = file.readAll();
    file.close();

    QString decodedText = decodeWindows1250(rawData);
    editor->setPlainText(decodedText);
    qDebug() << "[DEBUG] Windows-1250 decoding applied for file:" << filePath;
}

// Decode Windows-1250 encoded data
QString Interpret_As_windows_1250::decodeWindows1250(const QByteArray& rawData) {
    QString result;

    for (unsigned char byte : rawData) {
        auto it = windows1250Table.find(byte);
        if (it != windows1250Table.end()) {
            result.append(it->second);
        } else {
            result.append(QChar(0xFFFD)); // Replacement character for unknown bytes
        }
    }

    return result;
}

// Windows-1250 encoding table
const std::unordered_map<unsigned char, QChar> Interpret_As_windows_1250::windows1250Table = {
    {0x80, QChar(0x20AC)}, // Euro Sign
    {0x81, QChar(0xFFFD)}, // Undefined
    {0x82, QChar(0x201A)}, // Single Low-9 Quotation Mark
    {0x83, QChar(0xFFFD)}, // Undefined
    {0x84, QChar(0x201E)}, // Double Low-9 Quotation Mark
    {0x85, QChar(0x2026)}, // Horizontal Ellipsis
    {0x86, QChar(0x2020)}, // Dagger
    {0x87, QChar(0x2021)}, // Double Dagger
    {0x88, QChar(0xFFFD)}, // Undefined
    {0x89, QChar(0x2030)}, // Per Mille Sign
    {0xA0, QChar(0x00A0)}, // Non-breaking Space
    {0xA1, QChar(0x02C7)}, // Caron
    {0xA2, QChar(0x02D8)}, // Breve
    {0xA3, QChar(0x0141)}, // Latin Capital Letter L with Stroke
    {0xA4, QChar(0x00A4)}, // Currency Sign
    {0xA5, QChar(0x013D)}, // Latin Capital Letter L with Caron
    {0xA6, QChar(0x015A)}, // Latin Capital Letter S with Acute
    {0xA7, QChar(0x00A7)}, // Section Sign
    {0xA8, QChar(0x00A8)}, // Diaeresis
    {0xA9, QChar(0x0160)}, // Latin Capital Letter S with Caron
    {0xAA, QChar(0x015E)}, // Latin Capital Letter S with Cedilla
    {0xAB, QChar(0x0164)}, // Latin Capital Letter T with Caron
    {0xAC, QChar(0x0179)}, // Latin Capital Letter Z with Acute
    {0xAD, QChar(0x00AD)}, // Soft Hyphen
    {0xAE, QChar(0x017D)}, // Latin Capital Letter Z with Caron
    {0xAF, QChar(0x017B)}, // Latin Capital Letter Z with Dot Above
    {0xB0, QChar(0x00B0)}, // Degree Sign
    {0xB1, QChar(0x00B1)}, // Plus-Minus Sign
    {0xB2, QChar(0x02DB)}, // Ogonek
    {0xB3, QChar(0x0142)}, // Latin Small Letter L with Stroke
    {0xB4, QChar(0x00B4)}, // Acute Accent
    {0xB5, QChar(0x013E)}, // Latin Small Letter L with Caron
    {0xB6, QChar(0x015B)}, // Latin Small Letter S with Acute
    {0xB7, QChar(0x02C7)}, // Caron
    {0xB8, QChar(0x00B8)}, // Cedilla
    {0xB9, QChar(0x0161)}, // Latin Small Letter S with Caron
    {0xBA, QChar(0x015F)}, // Latin Small Letter S with Cedilla
    {0xBB, QChar(0x0165)}, // Latin Small Letter T with Caron
    {0xBC, QChar(0x017A)}, // Latin Small Letter Z with Acute
    {0xBD, QChar(0x02DD)}, // Double Acute Accent
    {0xBE, QChar(0x017E)}, // Latin Small Letter Z with Caron
    {0xBF, QChar(0x017C)}, // Latin Small Letter Z with Dot Above
    {0xC0, QChar(0x0154)}, // Latin Capital Letter R with Acute
    {0xC1, QChar(0x00C1)}, // Latin Capital Letter A with Acute
    {0xC2, QChar(0x00C2)}, // Latin Capital Letter A with Circumflex
    {0xC3, QChar(0x0102)}, // Latin Capital Letter A with Breve
    {0xC4, QChar(0x00C4)}, // Latin Capital Letter A with Diaeresis
    {0xC5, QChar(0x0139)}, // Latin Capital Letter L with Acute
    {0xC6, QChar(0x0106)}, // Latin Capital Letter C with Acute
    {0xC7, QChar(0x00C7)}, // Latin Capital Letter C with Cedilla
    {0xC8, QChar(0x010C)}, // Latin Capital Letter C with Caron
    {0xC9, QChar(0x00C9)}, // Latin Capital Letter E with Acute
    // Complete the remaining entries...
};
