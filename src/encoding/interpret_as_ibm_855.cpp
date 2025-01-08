#include "../codeeditor.h"
#include "interpret_as_ibm_855.h"
#include <QFile>
#include <QDebug>

// Singleton instance
Interpret_As_IBM_855& Interpret_As_IBM_855::instance() {
    static Interpret_As_IBM_855 instance;
    return instance;
}

// EBCDIC to Unicode mapping for IBM-855 (Cyrillic EBCDIC)
const std::unordered_map<unsigned char, QChar> Interpret_As_IBM_855::ebcdicTable = {
    {0x80, QChar(0x0452)},  // ђ
    {0x81, QChar(0x0453)},  // ғ
    {0x82, QChar(0x0451)},  // ё
    {0x83, QChar(0x0454)},  // є
    {0x84, QChar(0x0455)},  // ї
    {0x85, QChar(0x0456)},  // і
    {0x86, QChar(0x0457)},  // ґ
    {0x87, QChar(0x0458)},  // ј
    {0x88, QChar(0x0459)},  // љ
    {0x89, QChar(0x045A)},  // њ
    {0x8A, QChar(0x045B)},  // ћ
    {0x8B, QChar(0x045C)},  // ќ
    {0x8C, QChar(0x045E)},  // ў
    {0x8D, QChar(0x045F)},  // дз
    {0x8E, QChar(0x2116)},  // №
    {0x8F, QChar(0x0450)},  // ѐ
    {0x90, QChar(0x0401)},  // Ё
    {0x91, QChar(0x0402)},  // Ђ
    {0x92, QChar(0x0403)},  // Ѓ
    {0x93, QChar(0x0404)},  // Є
    {0x94, QChar(0x0405)},  // Ѕ
    {0x95, QChar(0x0406)},  // І
    {0x96, QChar(0x0407)},  // Ї
    {0x97, QChar(0x0408)},  // Ј
    {0x98, QChar(0x0409)},  // Љ
    {0x99, QChar(0x040A)},  // Њ
    {0x9A, QChar(0x040B)},  // Ћ
    {0x9B, QChar(0x040C)},  // Ќ
    {0x9C, QChar(0x040E)},  // Ў
    {0x9D, QChar(0x040F)},  // Џ
};

// Constructor
Interpret_As_IBM_855::Interpret_As_IBM_855() {}

// Main execution function
void Interpret_As_IBM_855::execute(QPlainTextEdit* editor) {
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

    QString decodedText = decodeIBM855(rawData);
    editor->setPlainText(decodedText);
    qDebug() << "[DEBUG] IBM-855 Decoding applied for file:" << filePath;
}

// Decode raw data from IBM-855 encoding
QString Interpret_As_IBM_855::decodeIBM855(const QByteArray& rawData) {
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
