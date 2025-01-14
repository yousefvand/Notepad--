#include "../codeeditor.h"
#include "interpret_as_ibm_862.h"
#include <QFile>
#include <QDebug>

// Singleton instance
Interpret_As_IBM_862& Interpret_As_IBM_862::instance() {
    static Interpret_As_IBM_862 instance;
    return instance;
}

// EBCDIC to Unicode mapping for IBM-862 (Hebrew EBCDIC)
const std::unordered_map<unsigned char, QChar> Interpret_As_IBM_862::ebcdicTable = {
    {0x80, QChar(0x05D0)},  // א (Alef)
    {0x81, QChar(0x05D1)},  // ב (Bet)
    {0x82, QChar(0x05D2)},  // ג (Gimel)
    {0x83, QChar(0x05D3)},  // ד (Dalet)
    {0x84, QChar(0x05D4)},  // ה (He)
    {0x85, QChar(0x05D5)},  // ו (Vav)
    {0x86, QChar(0x05D6)},  // ז (Zayin)
    {0x87, QChar(0x05D7)},  // ח (Het)
    {0x88, QChar(0x05D8)},  // ט (Tet)
    {0x89, QChar(0x05D9)},  // י (Yod)
    {0x8A, QChar(0x05DA)},  // ך (Final Kaf)
    {0x8B, QChar(0x05DB)},  // כ (Kaf)
    {0x8C, QChar(0x05DC)},  // ל (Lamed)
    {0x8D, QChar(0x05DD)},  // ם (Final Mem)
    {0x8E, QChar(0x05DE)},  // מ (Mem)
    {0x8F, QChar(0x05DF)},  // ן (Final Nun)
    {0x90, QChar(0x05E0)},  // נ (Nun)
    {0x91, QChar(0x05E1)},  // ס (Samekh)
    {0x92, QChar(0x05E2)},  // ע (Ayin)
    {0x93, QChar(0x05E3)},  // ף (Final Pe)
    {0x94, QChar(0x05E4)},  // פ (Pe)
    {0x95, QChar(0x05E5)},  // ץ (Final Tsadi)
    {0x96, QChar(0x05E6)},  // צ (Tsadi)
    {0x97, QChar(0x05E7)},  // ק (Qof)
    {0x98, QChar(0x05E8)},  // ר (Resh)
    {0x99, QChar(0x05E9)},  // ש (Shin)
    {0x9A, QChar(0x05EA)},  // ת (Tav)
    {0x9B, QChar(0x0020)},  // Space
    {0x9C, QChar(0x002E)},  // . (Period)
    {0x9D, QChar(0x002C)},  // , (Comma)
    {0x9E, QChar(0x003A)},  // : (Colon)
    {0x9F, QChar(0x003B)},  // ; (Semicolon)
};

// Constructor
Interpret_As_IBM_862::Interpret_As_IBM_862() {}

// Main execution function
void Interpret_As_IBM_862::execute(QPlainTextEdit* editor) {
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

    QString decodedText = decodeIBM862(rawData);
    editor->setPlainText(decodedText);
    qDebug() << "[DEBUG] IBM-862 Decoding applied for file:" << filePath;
}

// Decode raw data from IBM-862 encoding
QString Interpret_As_IBM_862::decodeIBM862(const QByteArray& rawData) {
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
