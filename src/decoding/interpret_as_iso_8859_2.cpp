#include "interpret_as_iso_8859_2.h"
#include <QFile>
#include <QDebug>
#include <QPlainTextEdit>
#include "../codeeditor.h"

// Singleton instance
Interpret_As_ISO_8859_2& Interpret_As_ISO_8859_2::instance() {
    static Interpret_As_ISO_8859_2 instance;
    return instance;
}

// ISO-8859-2 to Unicode mapping table (partial, extend as needed)
const QMap<uint8_t, QChar> iso88592ToUnicode = {
    { 0xA1, QChar(0x104) },  // Ą
    { 0xA2, QChar(0x2D8) },  // ˘
    { 0xA3, QChar(0x141) },  // Ł
    { 0xA5, QChar(0x13D) },  // Ľ
    { 0xA6, QChar(0x15A) },  // Ś
    { 0xA9, QChar(0x160) },  // Š
    { 0xAA, QChar(0x15E) },  // Ş
    { 0xAB, QChar(0x164) },  // Ť
    { 0xAC, QChar(0x179) },  // Ź
    { 0xAE, QChar(0x17D) },  // Ž
    { 0xAF, QChar(0x17B) },  // Ż
    { 0xB1, QChar(0x105) },  // ą
    { 0xB2, QChar(0x2DB) },  // ˛
    { 0xB3, QChar(0x142) },  // ł
    { 0xB5, QChar(0x13E) },  // ľ
    { 0xB6, QChar(0x15B) },  // ś
    { 0xB7, QChar(0x2C7) },  // ˇ
    { 0xB9, QChar(0x161) },  // š
    { 0xBA, QChar(0x15F) },  // ş
    { 0xBB, QChar(0x165) },  // ť
    { 0xBC, QChar(0x17A) },  // ź
    { 0xBD, QChar(0x2DD) },  // ˝
    { 0xBE, QChar(0x17E) },  // ž
    { 0xBF, QChar(0x17C) },  // ż
};

// Decode ISO-8859-2 manually
QString Interpret_As_ISO_8859_2::decodeISO88592(const QByteArray& iso88592Data) {
    QString result;

    for (char byte : iso88592Data) {
        uint8_t isoChar = static_cast<uint8_t>(byte);
        if (isoChar < 0x80) {
            result.append(QChar(isoChar));  // ASCII characters
        } else {
            // Map using ISO-8859-2 lookup table
            result.append(iso88592ToUnicode.value(isoChar, QChar(isoChar)));
        }
    }

    return result;
}

// Execute Interpretation
void Interpret_As_ISO_8859_2::execute(QPlainTextEdit* editor) {
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

    QByteArray iso88592Data = file.readAll();
    file.close();

    // Debugging: Print Raw Bytes
    qDebug() << "[DEBUG] Raw Bytes (Hex):" << iso88592Data.toHex();

    QString decodedText = decodeISO88592(iso88592Data);
    editor->setPlainText(decodedText);

    qDebug() << "[DEBUG] Decoding applied for file:" << filePath;
}
