#include "../codeeditor.h"
#include "interpret_as_ibm_871.h"
#include <QFile>
#include <QDebug>

// Singleton instance
Interpret_As_IBM_871& Interpret_As_IBM_871::instance() {
    static Interpret_As_IBM_871 instance;
    return instance;
}

// EBCDIC to Unicode mapping for IBM-871 (Icelandic EBCDIC)
const std::unordered_map<unsigned char, QChar> Interpret_As_IBM_871::ebcdicTable = {
    {0x40, QChar(0x0020)},  // Space
    {0x41, QChar(0x00C1)},  // Á
    {0x42, QChar(0x00C2)},  // Â
    {0x43, QChar(0x00C9)},  // É
    {0x44, QChar(0x00CD)},  // Í
    {0x45, QChar(0x00D3)},  // Ó
    {0x46, QChar(0x00DA)},  // Ú
    {0x47, QChar(0x00DD)},  // Ý
    {0x48, QChar(0x00E1)},  // á
    {0x49, QChar(0x00E2)},  // â
    {0x4A, QChar(0x00E9)},  // é
    {0x4B, QChar(0x00ED)},  // í
    {0x4C, QChar(0x00F3)},  // ó
    {0x4D, QChar(0x00FA)},  // ú
    {0x4E, QChar(0x00FD)},  // ý
    {0x4F, QChar(0x00FE)},  // þ
    {0x50, QChar(0x00D0)},  // Ð
    {0x51, QChar(0x00F0)},  // ð
    {0x5A, QChar(0x00C0)},  // À
    {0x5B, QChar(0x00C3)},  // Ã
    {0x5C, QChar(0x00E3)},  // ã
    {0x5F, QChar(0x00DF)},  // ß
    {0x6A, QChar(0x00F6)},  // ö
    {0x7A, QChar(0x00D6)},  // Ö
    {0x7B, QChar(0x00C4)},  // Ä
    {0x7C, QChar(0x00E4)},  // ä
    {0x7E, QChar(0x00DC)},  // Ü
    {0x7F, QChar(0x00FC)},  // ü
};

// Constructor
Interpret_As_IBM_871::Interpret_As_IBM_871() {}

// Main execution function
void Interpret_As_IBM_871::execute(QPlainTextEdit* editor) {
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

    QString decodedText = decodeIBM871(rawData);
    editor->setPlainText(decodedText);
    qDebug() << "[DEBUG] IBM-871 Decoding applied for file:" << filePath;
}

// Decode raw data from IBM-871 encoding
QString Interpret_As_IBM_871::decodeIBM871(const QByteArray& rawData) {
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
