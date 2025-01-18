#include "interpret_as_iso_8859_15.h"
#include <QFile>
#include <QDebug>
#include <QPlainTextEdit>
#include "../codeeditor.h"

// Singleton instance
Interpret_As_ISO_8859_15& Interpret_As_ISO_8859_15::instance() {
    static Interpret_As_ISO_8859_15 instance;
    return instance;
}

// ISO-8859-15 to Unicode mapping table for differences from ISO-8859-1
const QMap<uint8_t, QChar> iso885915ToUnicode = {
    { 0xA4, QChar(0x20AC) },  // € - Euro Sign
    { 0xA6, QChar(0x0160) },  // Š - Latin Capital Letter S with Caron
    { 0xA8, QChar(0x0161) },  // š - Latin Small Letter S with Caron
    { 0xB4, QChar(0x017D) },  // Ž - Latin Capital Letter Z with Caron
    { 0xB8, QChar(0x017E) },  // ž - Latin Small Letter Z with Caron
    { 0xBC, QChar(0x0152) },  // Œ - Latin Capital Ligature OE
    { 0xBD, QChar(0x0153) },  // œ - Latin Small Ligature OE
    { 0xBE, QChar(0x0178) },  // Ÿ - Latin Capital Letter Y with Diaeresis
};

// Decode ISO-8859-15 manually
QString Interpret_As_ISO_8859_15::decodeISO885915(const QByteArray& iso885915Data) {
    QString result;

    for (char byte : iso885915Data) {
        uint8_t isoChar = static_cast<uint8_t>(byte);
        if (isoChar < 0x80) {
            result.append(QChar(isoChar));  // ASCII characters (Direct mapping)
        } else {
            // Map ISO-8859-15 specific characters, fallback to original byte
            result.append(iso885915ToUnicode.value(isoChar, QChar(isoChar)));
        }
    }

    return result;
}

// Execute Interpretation
void Interpret_As_ISO_8859_15::execute(QPlainTextEdit* editor) {
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

    QByteArray iso885915Data = file.readAll();
    file.close();

    // Debugging: Print Raw Bytes
    qDebug() << "[DEBUG] Raw Bytes (Hex):" << iso885915Data.toHex();

    QString decodedText = decodeISO885915(iso885915Data);
    editor->setPlainText(decodedText);

    qDebug() << "[DEBUG] ISO-8859-15 Decoding applied for file:" << filePath;
}
