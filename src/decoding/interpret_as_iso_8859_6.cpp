#include "interpret_as_iso_8859_6.h"
#include <QFile>
#include <QDebug>
#include <QPlainTextEdit>
#include "../codeeditor.h"

// Singleton instance
Interpret_As_ISO_8859_6& Interpret_As_ISO_8859_6::instance() {
    static Interpret_As_ISO_8859_6 instance;
    return instance;
}

// ISO-8859-6 to Unicode mapping (Extended to handle more cases)
const QMap<uint8_t, QChar> iso88596ToUnicode = {
    { 0xA4, QChar(0x00A4) },  // Currency Sign
    { 0xAC, QChar(0x060C) },  // Arabic Comma
    { 0xAD, QChar(0x00AD) },  // Soft Hyphen
    { 0xBB, QChar(0x061B) },  // Arabic Semicolon
    { 0xBF, QChar(0x061F) },  // Arabic Question Mark
    { 0xC1, QChar(0x0621) },  // Arabic Letter Hamza
    { 0xC2, QChar(0x0622) },  // Alef with Madda Above
    { 0xC3, QChar(0x0623) },  // Alef with Hamza Above
    { 0xC4, QChar(0x0624) },  // Waw with Hamza Above
    { 0xC5, QChar(0x0625) },  // Alef with Hamza Below
    { 0xC6, QChar(0x0626) },  // Yeh with Hamza Above
    { 0xC7, QChar(0x0627) },  // Arabic Alef
    { 0xC8, QChar(0x0628) },  // Arabic Beh
    { 0xC9, QChar(0x0629) },  // Teh Marbuta
    { 0xCA, QChar(0x062A) },  // Teh
    { 0xCB, QChar(0x062B) },  // Theh
    { 0xCC, QChar(0x062C) },  // Jeem
    { 0xCD, QChar(0x062D) },  // Hah
    { 0xCE, QChar(0x062E) },  // Khah
    { 0xCF, QChar(0x062F) },  // Dal
    { 0xD0, QChar(0x0630) },  // Thal
    { 0xD1, QChar(0x0631) },  // Reh
    { 0xD2, QChar(0x0632) },  // Zain
    { 0xD3, QChar(0x0633) },  // Seen
    { 0xD4, QChar(0x0634) },  // Sheen
    { 0xD5, QChar(0x0635) },  // Sad
    { 0xD6, QChar(0x0636) },  // Dad
    { 0xD7, QChar(0x0637) },  // Tah
    { 0xD8, QChar(0x0638) },  // Zah
    { 0xD9, QChar(0x0639) },  // Ain
    { 0xDA, QChar(0x063A) },  // Ghain
    { 0xE0, QChar(0x0640) },  // Tatweel
    { 0xE1, QChar(0x0641) },  // Feh
    { 0xE2, QChar(0x0642) },  // Qaf
    { 0xE3, QChar(0x0643) },  // Kaf
    { 0xE4, QChar(0x0644) },  // Lam
    { 0xE5, QChar(0x0645) },  // Meem
    { 0xE6, QChar(0x0646) },  // Noon
    { 0xE7, QChar(0x0647) },  // Heh
    { 0xE8, QChar(0x0648) },  // Waw
    { 0xE9, QChar(0x0649) },  // Alef Maksura
    { 0xEA, QChar(0x064A) }   // Yeh
};

// Decode ISO-8859-6 manually
QString Interpret_As_ISO_8859_6::decodeISO88596(const QByteArray& iso88596Data) {
    QString result;

    for (char byte : iso88596Data) {
        uint8_t isoChar = static_cast<uint8_t>(byte);

        if (isoChar < 0x80) {
            // ASCII range (direct mapping)
            result.append(QChar(isoChar));
        } else {
            // Map ISO-8859-6 characters, fallback to replacement char if not found
            result.append(iso88596ToUnicode.value(isoChar, QChar(0xFFFD)));  // � for unmapped characters
        }
    }

    return result;
}

// Execute ISO-8859-6 Interpretation
void Interpret_As_ISO_8859_6::execute(QPlainTextEdit* editor) {
    if (!editor) {
        qWarning() << "[ERROR] No editor instance provided.";
        return;
    }

    CodeEditor* codeEditor = qobject_cast<CodeEditor*>(editor);
    if (!codeEditor) {
        qWarning() << "[ERROR] Editor is not a CodeEditor instance.";
        return;
    }

    QString filePath = codeEditor->filePath();
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "[ERROR] Cannot open file: " << filePath;
        return;
    }

    QByteArray iso88596Data = file.readAll();
    file.close();

    QString decodedText = decodeISO88596(iso88596Data);
    editor->setPlainText(decodedText);

    qDebug() << "[DEBUG] ISO-8859-6 Decoding Applied to File:" << filePath;
}
