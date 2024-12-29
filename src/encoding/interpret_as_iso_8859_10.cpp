#include "interpret_as_iso_8859_10.h"
#include <QFile>
#include <QDebug>
#include <QPlainTextEdit>
#include "../codeeditor.h"

// Singleton instance
Interpret_As_ISO_8859_10& Interpret_As_ISO_8859_10::instance() {
    static Interpret_As_ISO_8859_10 instance;
    return instance;
}

// ISO-8859-10 to Unicode mapping table (partial, extend as needed)
const QMap<uint8_t, QChar> iso885910ToUnicode = {
    { 0xA1, QChar(0x0104) },  // Ą
    { 0xA2, QChar(0x0112) },  // Ē
    { 0xA3, QChar(0x0122) },  // Ģ
    { 0xA5, QChar(0x0136) },  // Ķ
    { 0xA6, QChar(0x012A) },  // Ī
    { 0xA8, QChar(0x014A) },  // Ŋ
    { 0xAA, QChar(0x014C) },  // Ō
    { 0xAF, QChar(0x0172) },  // Ų
    { 0xB3, QChar(0x0105) },  // ą
    { 0xB4, QChar(0x0113) },  // ē
    { 0xB6, QChar(0x012B) },  // ī
    { 0xB8, QChar(0x014B) },  // ŋ
    { 0xBB, QChar(0x014D) },  // ō
    { 0xBF, QChar(0x0173) },  // ų
    { 0xC5, QChar(0x0160) },  // Š
    { 0xC6, QChar(0x0166) },  // Ŧ
    { 0xD3, QChar(0x0161) },  // š
    { 0xD6, QChar(0x0167) },  // ŧ
};

// Decode ISO-8859-10 manually
QString Interpret_As_ISO_8859_10::decodeISO885910(const QByteArray& iso885910Data) {
    QString result;

    for (char byte : iso885910Data) {
        uint8_t isoChar = static_cast<uint8_t>(byte);
        if (isoChar < 0x80) {
            result.append(QChar(isoChar));  // ASCII characters (Direct mapping)
        } else {
            // Map ISO-8859-10 specific characters, fallback to original byte
            result.append(iso885910ToUnicode.value(isoChar, QChar(isoChar)));
        }
    }

    return result;
}

// Execute Interpretation
void Interpret_As_ISO_8859_10::execute(QPlainTextEdit* editor) {
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

    QByteArray iso885910Data = file.readAll();
    file.close();

    // Debugging: Print Raw Bytes
    qDebug() << "[DEBUG] Raw Bytes (Hex):" << iso885910Data.toHex();

    QString decodedText = decodeISO885910(iso885910Data);
    editor->setPlainText(decodedText);

    qDebug() << "[DEBUG] ISO-8859-10 Decoding applied for file:" << filePath;
}
