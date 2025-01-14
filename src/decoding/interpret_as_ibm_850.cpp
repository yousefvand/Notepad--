#include "../codeeditor.h"
#include "interpret_as_ibm_850.h"
#include <QFile>
#include <QDebug>

// Singleton instance
Interpret_As_IBM_850& Interpret_As_IBM_850::instance() {
    static Interpret_As_IBM_850 instance;
    return instance;
}

// EBCDIC to Unicode mapping for IBM-850 (Latin-1)
const std::unordered_map<unsigned char, QChar> Interpret_As_IBM_850::ebcdicTable = {
    {0x80, QChar(0x00C7)},  // Ç
    {0x81, QChar(0x00FC)},  // ü
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
    {0x90, QChar(0x00C9)},  // É
    {0x91, QChar(0x00E6)},  // æ
    {0x92, QChar(0x00C6)},  // Æ
    {0x93, QChar(0x00F4)},  // ô
    {0x94, QChar(0x00F6)},  // ö
    {0x95, QChar(0x00F2)},  // ò
    {0x96, QChar(0x00FB)},  // û
    {0x97, QChar(0x00F9)},  // ù
    {0x98, QChar(0x00FF)},  // ÿ
    {0x99, QChar(0x00D6)},  // Ö
    {0x9A, QChar(0x00DC)},  // Ü
    {0x9B, QChar(0x00A2)},  // ¢
    {0x9C, QChar(0x00A3)},  // £
    {0x9D, QChar(0x00A5)},  // ¥
    {0x9E, QChar(0x20A7)},  // ₧
    {0x9F, QChar(0x0192)},  // ƒ
    {0xA0, QChar(0x00E1)},  // á
    {0xA1, QChar(0x00ED)},  // í
    {0xA2, QChar(0x00F3)},  // ó
    {0xA3, QChar(0x00FA)},  // ú
    {0xA4, QChar(0x00F1)},  // ñ
    {0xA5, QChar(0x00D1)},  // Ñ
    {0xA6, QChar(0x00AA)},  // ª
    {0xA7, QChar(0x00BA)},  // º
};

// Constructor
Interpret_As_IBM_850::Interpret_As_IBM_850() {}

// Main execution function
void Interpret_As_IBM_850::execute(QPlainTextEdit* editor) {
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

    QString decodedText = decodeIBM850(rawData);
    editor->setPlainText(decodedText);
    qDebug() << "[DEBUG] IBM-850 Decoding applied for file:" << filePath;
}

// Decode raw data from IBM-850 encoding
QString Interpret_As_IBM_850::decodeIBM850(const QByteArray& rawData) {
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
