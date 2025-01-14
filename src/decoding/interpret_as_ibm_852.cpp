#include "../codeeditor.h"
#include "interpret_as_ibm_852.h"
#include <QFile>
#include <QDebug>

// Singleton instance
Interpret_As_IBM_852& Interpret_As_IBM_852::instance() {
    static Interpret_As_IBM_852 instance;
    return instance;
}

// EBCDIC to Unicode mapping for IBM-852 (Central European)
const std::unordered_map<unsigned char, QChar> Interpret_As_IBM_852::ebcdicTable = {
    {0x80, QChar(0x0104)},  // Ą
    {0x81, QChar(0x00FC)},  // ü
    {0x82, QChar(0x00E9)},  // é
    {0x83, QChar(0x0101)},  // ā
    {0x84, QChar(0x00E4)},  // ä
    {0x85, QChar(0x0123)},  // ģ
    {0x86, QChar(0x00E5)},  // å
    {0x87, QChar(0x0107)},  // ć
    {0x88, QChar(0x0142)},  // ł
    {0x89, QChar(0x0113)},  // ē
    {0x8A, QChar(0x010D)},  // č
    {0x8B, QChar(0x0119)},  // ę
    {0x8C, QChar(0x0117)},  // ė
    {0x8D, QChar(0x0106)},  // Ć
    {0x8E, QChar(0x00C4)},  // Ä
    {0x8F, QChar(0x00C5)},  // Å
    {0x90, QChar(0x0141)},  // Ł
    {0x91, QChar(0x00E6)},  // æ
    {0x92, QChar(0x00C6)},  // Æ
    {0x93, QChar(0x0112)},  // Ē
    {0x94, QChar(0x00F6)},  // ö
    {0x95, QChar(0x00F3)},  // ó
    {0x96, QChar(0x0144)},  // ń
    {0x97, QChar(0x0143)},  // Ń
    {0x98, QChar(0x00D6)},  // Ö
    {0x99, QChar(0x00DC)},  // Ü
    {0x9A, QChar(0x00A2)},  // ¢
    {0x9B, QChar(0x015A)},  // Ś
    {0x9C, QChar(0x015B)},  // ś
    {0x9D, QChar(0x00D3)},  // Ó
    {0x9E, QChar(0x0179)},  // Ź
    {0x9F, QChar(0x017A)},  // ź
    {0xA0, QChar(0x00E1)},  // á
    {0xA1, QChar(0x00ED)},  // í
    {0xA2, QChar(0x00F3)},  // ó
    {0xA3, QChar(0x00FA)},  // ú
    {0xA4, QChar(0x00F1)},  // ñ
    {0xA5, QChar(0x00D1)},  // Ñ
    {0xA6, QChar(0x015E)},  // Ş
    {0xA7, QChar(0x015F)},  // ş
};

// Constructor
Interpret_As_IBM_852::Interpret_As_IBM_852() {}

// Main execution function
void Interpret_As_IBM_852::execute(QPlainTextEdit* editor) {
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

    QString decodedText = decodeIBM852(rawData);
    editor->setPlainText(decodedText);
    qDebug() << "[DEBUG] IBM-852 Decoding applied for file:" << filePath;
}

// Decode raw data from IBM-852 encoding
QString Interpret_As_IBM_852::decodeIBM852(const QByteArray& rawData) {
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
