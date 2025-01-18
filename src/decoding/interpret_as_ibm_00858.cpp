#include "../codeeditor.h"
#include "interpret_as_ibm_00858.h"
#include <QFile>
#include <QDebug>

// Singleton instance
Interpret_As_IBM_00858& Interpret_As_IBM_00858::instance() {
    static Interpret_As_IBM_00858 instance;
    return instance;
}

// Code page mapping for IBM-00858 (Latin-1 with Euro support)
const std::unordered_map<unsigned char, QChar> Interpret_As_IBM_00858::codePageTable = {
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
    {0x80, QChar(0x20AC)},  // €
    {0x81, QChar(0x0061)},  // a
    {0x82, QChar(0x00E9)},  // é
    {0x83, QChar(0x00E2)},  // â
    {0x84, QChar(0x00E4)},  // ä
    {0x85, QChar(0x00E0)},  // à
    {0x86, QChar(0x00E5)},  // å
    {0x87, QChar(0x00E7)},  // ç
    {0x88, QChar(0x00EA)},  // ê
    {0x89, QChar(0x00EB)},  // ë
    {0x8A, QChar(0x00E8)},  // è
    {0x8B, QChar(0x00EF)},  // ï
    {0x8C, QChar(0x00EE)},  // î
    {0x8D, QChar(0x00EC)},  // ì
    {0x8E, QChar(0x00C4)},  // Ä
    {0x8F, QChar(0x00C5)},  // Å
};

// Constructor
Interpret_As_IBM_00858::Interpret_As_IBM_00858() {}

// Main execution function
void Interpret_As_IBM_00858::execute(QPlainTextEdit* editor) {
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

    QString decodedText = decodeIBM00858(rawData);
    editor->setPlainText(decodedText);
    qDebug() << "[DEBUG] IBM-00858 Decoding applied for file:" << filePath;
}

// Decode raw data from IBM-00858 encoding
QString Interpret_As_IBM_00858::decodeIBM00858(const QByteArray& rawData) {
    QString result;

    for (unsigned char byte : rawData) {
        if (codePageTable.contains(byte)) {
            result.append(codePageTable.at(byte));
        } else {
            result.append(QChar(0xFFFD));  // Fallback for unmapped characters
        }
    }
    return result;
}
