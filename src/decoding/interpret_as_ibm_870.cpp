#include "../codeeditor.h"
#include "interpret_as_ibm_870.h"
#include <QFile>
#include <QDebug>

// Singleton instance
Interpret_As_IBM_870& Interpret_As_IBM_870::instance() {
    static Interpret_As_IBM_870 instance;
    return instance;
}

// EBCDIC to Unicode mapping for IBM-870 (Eastern European EBCDIC)
const std::unordered_map<unsigned char, QChar> Interpret_As_IBM_870::ebcdicTable = {
    {0x40, QChar(0x0020)},  // Space
    {0x4A, QChar(0x002E)},  // . (Full Stop)
    {0x5A, QChar(0x00A7)},  // § (Section Sign)
    {0x5F, QChar(0x0021)},  // ! (Exclamation Mark)
    {0x61, QChar(0x0104)},  // Ą (A with Ogonek)
    {0x62, QChar(0x00C1)},  // Á (A with Acute)
    {0x63, QChar(0x00C2)},  // Â (A with Circumflex)
    {0x64, QChar(0x0106)},  // Ć (C with Acute)
    {0x65, QChar(0x00C4)},  // Ä (A with Diaeresis)
    {0x66, QChar(0x00C5)},  // Å (A with Ring)
    {0x67, QChar(0x00C7)},  // Ç (C with Cedilla)
    {0x68, QChar(0x00C9)},  // É (E with Acute)
    {0x69, QChar(0x00D3)},  // Ó (O with Acute)
    {0x6A, QChar(0x0141)},  // Ł (L with Stroke)
    {0x6B, QChar(0x0143)},  // Ń (N with Acute)
    {0x6C, QChar(0x00D6)},  // Ö (O with Diaeresis)
    {0x6D, QChar(0x00DC)},  // Ü (U with Diaeresis)
    {0x6E, QChar(0x00D8)},  // Ø (O with Stroke)
    {0x7A, QChar(0x0118)},  // Ę (E with Ogonek)
    {0x7B, QChar(0x011A)},  // Ě (E with Caron)
    {0x7C, QChar(0x00DE)},  // Þ (Thorn)
    {0x7D, QChar(0x00D0)},  // Ð (Eth)
    {0x7E, QChar(0x00F0)},  // ð (eth)
};

// Constructor
Interpret_As_IBM_870::Interpret_As_IBM_870() {}

// Main execution function
void Interpret_As_IBM_870::execute(QPlainTextEdit* editor) {
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

    QString decodedText = decodeIBM870(rawData);
    editor->setPlainText(decodedText);
    qDebug() << "[DEBUG] IBM-870 Decoding applied for file:" << filePath;
}

// Decode raw data from IBM-870 encoding
QString Interpret_As_IBM_870::decodeIBM870(const QByteArray& rawData) {
    QString result;

    for (unsigned char byte : rawData) {
        if (ebcdicTable.contains(byte)) {
            result.append(ebcdicTable.at(byte));
        } else {
            result.append(QChar(0xFFFD));  // Fallback for unmapped characters
        }
    }
    return result;
}
