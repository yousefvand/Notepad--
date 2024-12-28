#include "interpret_as_iso_8859_14.h"
#include <QFile>
#include <QDebug>
#include <QPlainTextEdit>
#include "../codeeditor.h"

// Singleton instance
Interpret_As_ISO_8859_14& Interpret_As_ISO_8859_14::instance() {
    static Interpret_As_ISO_8859_14 instance;
    return instance;
}

// ISO-8859-14 to Unicode mapping table (partial, extend as needed)
const QMap<uint8_t, QChar> iso885914ToUnicode = {
    { 0xA1, QChar(0x1E02) },  // Ḃ
    { 0xA2, QChar(0x1E03) },  // ḃ
    { 0xA3, QChar(0x010A) },  // Ċ
    { 0xA4, QChar(0x010B) },  // ċ
    { 0xA5, QChar(0x1E0A) },  // Ḋ
    { 0xA6, QChar(0x1E09) },  // ḋ
    { 0xA7, QChar(0x1E1E) },  // Ḟ
    { 0xA8, QChar(0x1E1F) },  // ḟ
    { 0xA9, QChar(0x0120) },  // Ġ
    { 0xAA, QChar(0x0121) },  // ġ
    { 0xAB, QChar(0x1E40) },  // Ṁ
    { 0xAC, QChar(0x1E41) },  // ṁ
    { 0xAD, QChar(0x1E56) },  // Ṗ
    { 0xAE, QChar(0x1E57) },  // ṗ
    { 0xAF, QChar(0x1E60) },  // Ṡ
    { 0xB0, QChar(0x1E61) },  // ṡ
    { 0xB1, QChar(0x1E6A) },  // Ṫ
    { 0xB2, QChar(0x1E6B) },  // ṫ
    { 0xBB, QChar(0x00BB) },  // »
};

// Decode ISO-8859-14 manually
QString Interpret_As_ISO_8859_14::decodeISO885914(const QByteArray& iso885914Data) {
    QString result;

    for (char byte : iso885914Data) {
        uint8_t isoChar = static_cast<uint8_t>(byte);
        if (isoChar < 0x80) {
            result.append(QChar(isoChar));  // ASCII characters (Direct mapping)
        } else {
            // Map ISO-8859-14 specific characters, fallback to original byte
            result.append(iso885914ToUnicode.value(isoChar, QChar(isoChar)));
        }
    }

    return result;
}

// Execute Interpretation
void Interpret_As_ISO_8859_14::execute(QPlainTextEdit* editor) {
    if (!editor) {
        qWarning() << "[ERROR] No editor instance.";
        return;
    }

    CodeEditor* codeEditor = qobject_cast<CodeEditor*>(editor);
    if (!codeEditor) {
        qWarning() << "[ERROR] Editor is not a CodeEditor.";
        return;
    }

    QString filePath = codeEditor->filePath();
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "[ERROR] Cannot open file: " << filePath;
        return;
    }

    QByteArray iso885914Data = file.readAll();
    file.close();

    // Debugging: Print Raw Bytes
    qDebug() << "[DEBUG] Raw Bytes (Hex):" << iso885914Data.toHex();

    QString decodedText = decodeISO885914(iso885914Data);
    editor->setPlainText(decodedText);

    qDebug() << "[DEBUG] ISO-8859-14 Decoding applied for file:" << filePath;
}
