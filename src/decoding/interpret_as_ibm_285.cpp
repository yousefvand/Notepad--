#include "../codeeditor.h"
#include "interpret_as_ibm_285.h"
#include <QFile>
#include <QDebug>

// Singleton instance
Interpret_As_IBM_285& Interpret_As_IBM_285::instance() {
    static Interpret_As_IBM_285 instance;
    return instance;
}

// EBCDIC to Unicode mapping for IBM-285 (UK EBCDIC)
const std::unordered_map<unsigned char, QChar> Interpret_As_IBM_285::ebcdicTable = {
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
    {0x7B, QChar(0x00A3)},  // £
    {0x81, QChar(0x0061)},  // a
    {0x82, QChar(0x0062)},  // b
    {0x83, QChar(0x0063)},  // c
    {0x84, QChar(0x0064)},  // d
    {0x85, QChar(0x0065)},  // e
    {0x86, QChar(0x0066)},  // f
    {0x87, QChar(0x0067)},  // g
    {0x88, QChar(0x0068)},  // h
    {0x89, QChar(0x0069)},  // i
    {0x8A, QChar(0x006A)},  // j
    {0x8B, QChar(0x006B)},  // k
    {0x8C, QChar(0x006C)},  // l
    {0x8D, QChar(0x006D)},  // m
    {0x8E, QChar(0x006E)},  // n
    {0x8F, QChar(0x006F)},  // o
    {0x90, QChar(0x0070)},  // p
    {0x91, QChar(0x0071)},  // q
    {0x92, QChar(0x0072)},  // r
    {0x93, QChar(0x0073)},  // s
    {0x94, QChar(0x0074)},  // t
    {0x95, QChar(0x0075)},  // u
    {0x96, QChar(0x0076)},  // v
    {0x97, QChar(0x0077)},  // w
    {0x98, QChar(0x0078)},  // x
    {0x99, QChar(0x0079)},  // y
    {0x9A, QChar(0x007A)},  // z
};

// Constructor
Interpret_As_IBM_285::Interpret_As_IBM_285() {}

// Main execution function
void Interpret_As_IBM_285::execute(QPlainTextEdit* editor) {
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

    QString decodedText = decodeIBM285(rawData);
    editor->setPlainText(decodedText);
    qDebug() << "[DEBUG] IBM-285 Decoding applied for file:" << filePath;
}

// Decode raw data from IBM-285 encoding
QString Interpret_As_IBM_285::decodeIBM285(const QByteArray& rawData) {
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
