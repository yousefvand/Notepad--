#include "interpret_as_iso_8859_3.h"
#include <QFile>
#include <QDebug>
#include <QPlainTextEdit>
#include "../codeeditor.h"

// Singleton instance
Interpret_As_ISO_8859_3& Interpret_As_ISO_8859_3::instance() {
    static Interpret_As_ISO_8859_3 instance;
    return instance;
}

// ISO-8859-3 to Unicode mapping table (partial)
const QMap<uint8_t, QChar> iso88593ToUnicode = {
    { 0xA0, QChar(0x00A0) },  // No-Break Space
    { 0xA1, QChar(0x0126) },  // Ħ
    { 0xA2, QChar(0x02D8) },  // ˘
    { 0xA3, QChar(0x00A3) },  // £
    { 0xA4, QChar(0x00A4) },  // Currency sign
    { 0xA6, QChar(0x0124) },  // Ĥ
    { 0xA7, QChar(0x00A7) },  // Section sign
    { 0xA8, QChar(0x00A8) },  // Diaeresis
    { 0xA9, QChar(0x0130) },  // İ
    { 0xAA, QChar(0x015E) },  // Ş
    { 0xAB, QChar(0x011E) },  // Ğ
    { 0xAC, QChar(0x0134) },  // Ĵ
    { 0xAD, QChar(0x00AD) },  // Soft Hyphen
    { 0xAF, QChar(0x017B) },  // Ż
    { 0xB0, QChar(0x00B0) },  // Degree sign
    { 0xB1, QChar(0x0127) },  // ħ
    { 0xB2, QChar(0x00B2) },  // ²
    { 0xB3, QChar(0x00B3) },  // ³
    { 0xB4, QChar(0x00B4) },  // Acute accent
    { 0xB5, QChar(0x00B5) },  // µ
    { 0xB6, QChar(0x0125) },  // ĥ
    { 0xB7, QChar(0x00B7) },  // Middle dot
    { 0xB8, QChar(0x00B8) },  // Cedilla
    { 0xB9, QChar(0x0131) },  // ı
    { 0xBA, QChar(0x015F) },  // ş
    { 0xBB, QChar(0x011F) },  // ğ
    { 0xBC, QChar(0x0135) },  // ĵ
    { 0xBD, QChar(0x00BD) },  // ½
    { 0xBF, QChar(0x017C) },  // ż
};

// Decode ISO-8859-3 manually
QString Interpret_As_ISO_8859_3::decodeISO88593(const QByteArray& iso88593Data) {
    QString result;

    for (char byte : iso88593Data) {
        uint8_t isoChar = static_cast<uint8_t>(byte);
        if (isoChar < 0x80) {
            result.append(QChar(isoChar));  // ASCII characters
        } else {
            // Map using ISO-8859-3 lookup table
            result.append(iso88593ToUnicode.value(isoChar, QChar(isoChar)));
        }
    }

    return result;
}

// Execute Interpretation
void Interpret_As_ISO_8859_3::execute(QPlainTextEdit* editor) {
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

    QByteArray iso88593Data = file.readAll();
    file.close();

    // Debugging: Print Raw Bytes
    qDebug() << "[DEBUG] Raw Bytes (Hex):" << iso88593Data.toHex();

    QString decodedText = decodeISO88593(iso88593Data);
    editor->setPlainText(decodedText);

    qDebug() << "[DEBUG] Decoding applied for file:" << filePath;
}
