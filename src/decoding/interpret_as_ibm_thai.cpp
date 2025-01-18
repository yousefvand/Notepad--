#include "../codeeditor.h"
#include "interpret_as_ibm_thai.h"
#include <QFile>
#include <QDebug>

// Singleton instance
Interpret_As_IBM_Thai& Interpret_As_IBM_Thai::instance() {
    static Interpret_As_IBM_Thai instance;
    return instance;
}

// IBM Thai Code Page mapping
const std::unordered_map<unsigned char, QChar> Interpret_As_IBM_Thai::codePageTable = {
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
    {0xA1, QChar(0x0E01)},  // ก
    {0xA2, QChar(0x0E02)},  // ข
    {0xA3, QChar(0x0E03)},  // ฃ
    {0xA4, QChar(0x0E04)},  // ค
    {0xA5, QChar(0x0E05)},  // ฅ
    {0xA6, QChar(0x0E06)},  // ฆ
    {0xA7, QChar(0x0E07)},  // ง
    {0xA8, QChar(0x0E08)},  // จ
    {0xA9, QChar(0x0E09)},  // ฉ
    {0xAA, QChar(0x0E0A)},  // ช
    {0xAB, QChar(0x0E0B)},  // ซ
    {0xAC, QChar(0x0E0C)},  // ฌ
    {0xAD, QChar(0x0E0D)},  // ญ
    {0xAE, QChar(0x0E0E)},  // ฎ
    {0xAF, QChar(0x0E0F)},  // ฏ
    {0xB0, QChar(0x0E10)},  // ฐ
    {0xB1, QChar(0x0E11)},  // ฑ
    {0xB2, QChar(0x0E12)},  // ฒ
    {0xB3, QChar(0x0E13)},  // ณ
    {0xB4, QChar(0x0E14)},  // ด
    {0xB5, QChar(0x0E15)},  // ต
    {0xB6, QChar(0x0E16)},  // ถ
    {0xB7, QChar(0x0E17)},  // ท
    {0xB8, QChar(0x0E18)},  // ธ
    {0xB9, QChar(0x0E19)},  // น
    {0xBA, QChar(0x0E1A)},  // บ
    {0xBB, QChar(0x0E1B)},  // ป
    {0xBC, QChar(0x0E1C)},  // ผ
    {0xBD, QChar(0x0E1D)},  // ฝ
    {0xBE, QChar(0x0E1E)},  // พ
    {0xBF, QChar(0x0E1F)},  // ฟ
    {0xC0, QChar(0x0E20)},  // ภ
    {0xC1, QChar(0x0E21)},  // ม
    {0xC2, QChar(0x0E22)},  // ย
    {0xC3, QChar(0x0E23)},  // ร
    {0xC4, QChar(0x0E24)},  // ฤ
    {0xC5, QChar(0x0E25)},  // ล
    {0xC6, QChar(0x0E26)},  // ฦ
    {0xC7, QChar(0x0E27)},  // ว
    {0xC8, QChar(0x0E28)},  // ศ
    {0xC9, QChar(0x0E29)},  // ษ
    {0xCA, QChar(0x0E2A)},  // ส
    {0xCB, QChar(0x0E2B)},  // ห
    {0xCC, QChar(0x0E2C)},  // ฬ
    {0xCD, QChar(0x0E2D)},  // อ
    {0xCE, QChar(0x0E2E)},  // ฮ
    {0xCF, QChar(0x0E2F)},  // ฯ
};

// Constructor
Interpret_As_IBM_Thai::Interpret_As_IBM_Thai() {}

// Main execution function
void Interpret_As_IBM_Thai::execute(QPlainTextEdit* editor) {
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

    QString decodedText = decodeIBMThai(rawData);
    editor->setPlainText(decodedText);
    qDebug() << "[DEBUG] IBM-Thai Decoding applied for file:" << filePath;
}

// Decode raw data from IBM-Thai encoding
QString Interpret_As_IBM_Thai::decodeIBMThai(const QByteArray& rawData) {
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
