#include "../codeeditor.h"
#include "interpret_as_ibm_1026.h"
#include <QFile>
#include <QDebug>

// Singleton instance
Interpret_As_IBM_1026& Interpret_As_IBM_1026::instance() {
    static Interpret_As_IBM_1026 instance;
    return instance;
}

// EBCDIC to Unicode mapping for IBM-1026 (Turkish EBCDIC)
const std::unordered_map<unsigned char, QChar> Interpret_As_IBM_1026::ebcdicTable = {
    {0x41, QChar(0x0041)},  // A
    {0x42, QChar(0x0042)},  // B
    {0x43, QChar(0x0043)},  // C
    {0x44, QChar(0x0044)},  // D
    {0x45, QChar(0x0045)},  // E
    {0x46, QChar(0x0046)},  // F
    {0x47, QChar(0x0047)},  // G
    {0x48, QChar(0x0048)},  // H
    {0x49, QChar(0x0049)},  // I
    {0x4A, QChar(0x004A)},  // J
    {0x4B, QChar(0x004B)},  // K
    {0x4C, QChar(0x004C)},  // L
    {0x4D, QChar(0x004D)},  // M
    {0x4E, QChar(0x004E)},  // N
    {0x4F, QChar(0x004F)},  // O
    {0x50, QChar(0x0050)},  // P
    {0x51, QChar(0x0051)},  // Q
    {0x52, QChar(0x0052)},  // R
    {0x53, QChar(0x0053)},  // S
    {0x54, QChar(0x0054)},  // T
    {0x55, QChar(0x0055)},  // U
    {0x56, QChar(0x0056)},  // V
    {0x57, QChar(0x0057)},  // W
    {0x58, QChar(0x0058)},  // X
    {0x59, QChar(0x0059)},  // Y
    {0x5A, QChar(0x005A)},  // Z
    {0x7B, QChar(0x00E7)},  // ç
    {0x81, QChar(0x0061)},  // a
    {0x85, QChar(0x0065)},  // e
    {0x8C, QChar(0x00F6)},  // ö
    {0x90, QChar(0x00FC)},  // ü
    {0xA4, QChar(0x011F)},  // ğ
    {0xA7, QChar(0x0131)},  // ı
    {0xB1, QChar(0x015E)},  // Ş
    {0xD0, QChar(0x011E)},  // Ğ
    {0xDD, QChar(0x015F)},  // ş
};

// Constructor
Interpret_As_IBM_1026::Interpret_As_IBM_1026() {}

// Main execution function
void Interpret_As_IBM_1026::execute(QPlainTextEdit* editor) {
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

    QString decodedText = decodeIBM1026(rawData);
    editor->setPlainText(decodedText);
    qDebug() << "[DEBUG] IBM-1026 Decoding applied for file:" << filePath;
}

// Decode raw data from IBM-1026 encoding
QString Interpret_As_IBM_1026::decodeIBM1026(const QByteArray& rawData) {
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
