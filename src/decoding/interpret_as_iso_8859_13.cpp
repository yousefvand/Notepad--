#include "interpret_as_iso_8859_13.h"
#include <QFile>
#include <QDebug>
#include <QPlainTextEdit>
#include "../codeeditor.h"

// Singleton instance
Interpret_As_ISO_8859_13& Interpret_As_ISO_8859_13::instance() {
    static Interpret_As_ISO_8859_13 instance;
    return instance;
}

// ISO-8859-13 to Unicode mapping table (partial, extend as needed)
const QMap<uint8_t, QChar> iso885913ToUnicode = {
    { 0xA1, QChar(0x0104) },  // Ą
    { 0xA2, QChar(0x012E) },  // Į
    { 0xA3, QChar(0x0100) },  // Ā
    { 0xA5, QChar(0x0106) },  // Ć
    { 0xA6, QChar(0x0118) },  // Ę
    { 0xA8, QChar(0x0112) },  // Ē
    { 0xAA, QChar(0x0116) },  // Ė
    { 0xAF, QChar(0x0122) },  // Ģ
    { 0xB3, QChar(0x0101) },  // ā
    { 0xB4, QChar(0x0107) },  // ć
    { 0xB6, QChar(0x0119) },  // ę
    { 0xB8, QChar(0x0113) },  // ē
    { 0xBB, QChar(0x0117) },  // ė
    { 0xBF, QChar(0x0123) },  // ģ
    { 0xC5, QChar(0x0160) },  // Š
    { 0xC6, QChar(0x012A) },  // Ī
    { 0xC9, QChar(0x014C) },  // Ō
    { 0xD3, QChar(0x0161) },  // š
    { 0xD6, QChar(0x012B) },  // ī
    { 0xD9, QChar(0x014D) },  // ō
};

// Decode ISO-8859-13 manually
QString Interpret_As_ISO_8859_13::decodeISO885913(const QByteArray& iso885913Data) {
    QString result;

    for (char byte : iso885913Data) {
        uint8_t isoChar = static_cast<uint8_t>(byte);
        if (isoChar < 0x80) {
            result.append(QChar(isoChar));  // ASCII characters (Direct mapping)
        } else {
            // Map ISO-8859-13 specific characters, fallback to original byte
            result.append(iso885913ToUnicode.value(isoChar, QChar(isoChar)));
        }
    }

    return result;
}

// Execute Interpretation
void Interpret_As_ISO_8859_13::execute(QPlainTextEdit* editor) {
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

    QByteArray iso885913Data = file.readAll();
    file.close();

    // Debugging: Print Raw Bytes
    qDebug() << "[DEBUG] Raw Bytes (Hex):" << iso885913Data.toHex();

    QString decodedText = decodeISO885913(iso885913Data);
    editor->setPlainText(decodedText);

    qDebug() << "[DEBUG] ISO-8859-13 Decoding applied for file:" << filePath;
}
