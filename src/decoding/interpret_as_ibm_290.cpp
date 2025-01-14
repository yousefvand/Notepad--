#include "../codeeditor.h"
#include "interpret_as_ibm_290.h"
#include <QFile>
#include <QDebug>

// Singleton instance
Interpret_As_IBM_290& Interpret_As_IBM_290::instance() {
    static Interpret_As_IBM_290 instance;
    return instance;
}

// EBCDIC to Unicode mapping for IBM-290 (Japanese Katakana Extended)
const std::unordered_map<unsigned char, QChar> Interpret_As_IBM_290::ebcdicTable = {
    {0x41, QChar(0x30A1)},  // ァ (small a)
    {0x42, QChar(0x30A2)},  // ア (a)
    {0x43, QChar(0x30A3)},  // ィ (small i)
    {0x44, QChar(0x30A4)},  // イ (i)
    {0x45, QChar(0x30A5)},  // ゥ (small u)
    {0x46, QChar(0x30A6)},  // ウ (u)
    {0x47, QChar(0x30A7)},  // ェ (small e)
    {0x48, QChar(0x30A8)},  // エ (e)
    {0x49, QChar(0x30A9)},  // ォ (small o)
    {0x4A, QChar(0x30AA)},  // オ (o)
    {0x4B, QChar(0x30AB)},  // カ (ka)
    {0x4C, QChar(0x30AC)},  // ガ (ga)
    {0x4D, QChar(0x30AD)},  // キ (ki)
    {0x4E, QChar(0x30AE)},  // ギ (gi)
    {0x4F, QChar(0x30AF)},  // ク (ku)
    {0x50, QChar(0x30B0)},  // グ (gu)
    {0x51, QChar(0x30B1)},  // ケ (ke)
    {0x52, QChar(0x30B2)},  // ゲ (ge)
    {0x53, QChar(0x30B3)},  // コ (ko)
    {0x54, QChar(0x30B4)},  // ゴ (go)
    {0x55, QChar(0x30B5)},  // サ (sa)
    {0x56, QChar(0x30B6)},  // ザ (za)
    {0x57, QChar(0x30B7)},  // シ (shi)
    {0x58, QChar(0x30B8)},  // ジ (ji)
    {0x59, QChar(0x30B9)},  // ス (su)
    {0x5A, QChar(0x30BA)},  // ズ (zu)
};

// Constructor
Interpret_As_IBM_290::Interpret_As_IBM_290() {}

// Main execution function
void Interpret_As_IBM_290::execute(QPlainTextEdit* editor) {
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

    QString decodedText = decodeIBM290(rawData);
    editor->setPlainText(decodedText);
    qDebug() << "[DEBUG] IBM-290 Decoding applied for file:" << filePath;
}

// Decode raw data from IBM-290 encoding
QString Interpret_As_IBM_290::decodeIBM290(const QByteArray& rawData) {
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
