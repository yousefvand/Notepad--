#include "interpret_as_iso_8859_4.h"
#include <QFile>
#include <QDebug>
#include <QPlainTextEdit>
#include "../codeeditor.h"

// Singleton instance
Interpret_As_ISO_8859_4& Interpret_As_ISO_8859_4::instance() {
    static Interpret_As_ISO_8859_4 instance;
    return instance;
}

// ISO-8859-4 to Unicode mapping table (partial, extend as needed)
const QMap<uint8_t, QChar> iso88594ToUnicode = {
    { 0xA1, QChar(0x0104) },  // Ą
    { 0xA2, QChar(0x0138) },  // ĸ
    { 0xA3, QChar(0x0156) },  // Ŗ
    { 0xA4, QChar(0x00A4) },  // Currency sign
    { 0xA5, QChar(0x0128) },  // Ĩ
    { 0xA6, QChar(0x013B) },  // Ļ
    { 0xA7, QChar(0x00A7) },  // Section sign
    { 0xA8, QChar(0x00A8) },  // Diaeresis
    { 0xA9, QChar(0x0160) },  // Š
    { 0xAA, QChar(0x0112) },  // Ē
    { 0xAB, QChar(0x0122) },  // Ģ
    { 0xAC, QChar(0x0166) },  // Ŧ
    { 0xAD, QChar(0x00AD) },  // Soft Hyphen
    { 0xAE, QChar(0x017D) },  // Ž
    { 0xAF, QChar(0x00AF) },  // Macron
    { 0xB0, QChar(0x00B0) },  // Degree sign
    { 0xB1, QChar(0x0105) },  // ą
    { 0xB2, QChar(0x02DB) },  // Ogonek
    { 0xB3, QChar(0x0157) },  // ŗ
    { 0xB4, QChar(0x00B4) },  // Acute accent
    { 0xB5, QChar(0x0129) },  // į
    { 0xB6, QChar(0x013C) },  // ļ
    { 0xB7, QChar(0x02C7) },  // Caron
    { 0xB8, QChar(0x00B8) },  // Cedilla
    { 0xB9, QChar(0x0161) },  // š
    { 0xBA, QChar(0x0113) },  // ē
    { 0xBB, QChar(0x0123) },  // ģ
    { 0xBC, QChar(0x0167) },  // ŧ
    { 0xBD, QChar(0x014A) },  // Ŋ
    { 0xBE, QChar(0x017E) },  // ž
    { 0xBF, QChar(0x014B) },  // ŋ
};

// Decode ISO-8859-4 manually
QString Interpret_As_ISO_8859_4::decodeISO88594(const QByteArray& iso88594Data) {
    QString result;

    for (char byte : iso88594Data) {
        uint8_t isoChar = static_cast<uint8_t>(byte);
        if (isoChar < 0x80) {
            result.append(QChar(isoChar));  // ASCII characters
        } else {
            // Map using ISO-8859-4 lookup table
            result.append(iso88594ToUnicode.value(isoChar, QChar(isoChar)));
        }
    }

    return result;
}

// Execute Interpretation
void Interpret_As_ISO_8859_4::execute(QPlainTextEdit* editor) {
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

    QByteArray iso88594Data = file.readAll();
    file.close();

    // Debugging: Print Raw Bytes
    qDebug() << "[DEBUG] Raw Bytes (Hex):" << iso88594Data.toHex();

    QString decodedText = decodeISO88594(iso88594Data);
    editor->setPlainText(decodedText);

    qDebug() << "[DEBUG] Decoding applied for file:" << filePath;
}
