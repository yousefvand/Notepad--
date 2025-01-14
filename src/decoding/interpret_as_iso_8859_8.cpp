#include "interpret_as_iso_8859_8.h"
#include <QFile>
#include <QDebug>
#include <QPlainTextEdit>
#include "../codeeditor.h"

// Singleton instance
Interpret_As_ISO_8859_8& Interpret_As_ISO_8859_8::instance() {
    static Interpret_As_ISO_8859_8 instance;
    return instance;
}

// ISO-8859-8 to Unicode mapping table (Partial, focuses on Hebrew letters)
const QMap<uint8_t, QChar> iso88598ToUnicode = {
    { 0xDF, QChar(0x2017) },  // Double Low Line
    { 0xE0, QChar(0x05D0) },  // Aleph (א)
    { 0xE1, QChar(0x05D1) },  // Bet (ב)
    { 0xE2, QChar(0x05D2) },  // Gimel (ג)
    { 0xE3, QChar(0x05D3) },  // Dalet (ד)
    { 0xE4, QChar(0x05D4) },  // He (ה)
    { 0xE5, QChar(0x05D5) },  // Vav (ו)
    { 0xE6, QChar(0x05D6) },  // Zayin (ז)
    { 0xE7, QChar(0x05D7) },  // Het (ח)
    { 0xE8, QChar(0x05D8) },  // Tet (ט)
    { 0xE9, QChar(0x05D9) },  // Yod (י)
    { 0xEA, QChar(0x05DA) },  // Final Kaf (ך)
    { 0xEB, QChar(0x05DB) },  // Kaf (כ)
    { 0xEC, QChar(0x05DC) },  // Lamed (ל)
    { 0xED, QChar(0x05DD) },  // Final Mem (ם)
    { 0xEE, QChar(0x05DE) },  // Mem (מ)
    { 0xEF, QChar(0x05DF) },  // Final Nun (ן)
    { 0xF0, QChar(0x05E0) },  // Nun (נ)
    { 0xF1, QChar(0x05E1) },  // Samekh (ס)
    { 0xF2, QChar(0x05E2) },  // Ayin (ע)
    { 0xF3, QChar(0x05E3) },  // Final Pe (ף)
    { 0xF4, QChar(0x05E4) },  // Pe (פ)
    { 0xF5, QChar(0x05E5) },  // Final Tsade (ץ)
    { 0xF6, QChar(0x05E6) },  // Tsade (צ)
    { 0xF7, QChar(0x05E7) },  // Qof (ק)
    { 0xF8, QChar(0x05E8) },  // Resh (ר)
    { 0xF9, QChar(0x05E9) },  // Shin (ש)
    { 0xFA, QChar(0x05EA) },  // Tav (ת)
};

// Decode ISO-8859-8 manually
QString Interpret_As_ISO_8859_8::decodeISO88598(const QByteArray& iso88598Data) {
    QString result;

    for (char byte : iso88598Data) {
        uint8_t isoChar = static_cast<uint8_t>(byte);

        if (isoChar < 0x80) {
            // ASCII range (direct mapping)
            result.append(QChar(isoChar));
        } else {
            // Map ISO-8859-8 characters, fallback to replacement char if not found
            result.append(iso88598ToUnicode.value(isoChar, QChar(isoChar)));
        }
    }

    return result;
}

// Execute ISO-8859-8 Interpretation
void Interpret_As_ISO_8859_8::execute(QPlainTextEdit* editor) {
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

    QByteArray iso88598Data = file.readAll();
    file.close();

    QString decodedText = decodeISO88598(iso88598Data);
    editor->setPlainText(decodedText);

    qDebug() << "[DEBUG] ISO-8859-8 Decoding Applied to File:" << filePath;
}
