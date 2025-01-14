#include "interpret_as_iso_8859_7.h"
#include <QFile>
#include <QDebug>
#include <QPlainTextEdit>
#include "../codeeditor.h"

// Singleton instance
Interpret_As_ISO_8859_7& Interpret_As_ISO_8859_7::instance() {
    static Interpret_As_ISO_8859_7 instance;
    return instance;
}

// ISO-8859-7 to Unicode mapping table (Partial, focuses on Greek letters)
const QMap<uint8_t, QChar> iso88597ToUnicode = {
    { 0xA1, QChar(0x2018) },  // Left Single Quotation Mark
    { 0xA2, QChar(0x2019) },  // Right Single Quotation Mark
    { 0xA3, QChar(0x00A3) },  // Pound Sign
    { 0xA4, QChar(0x20AC) },  // Euro Sign
    { 0xA5, QChar(0x20AF) },  // Drachma Sign
    { 0xA6, QChar(0x00A6) },  // Broken Bar
    { 0xA7, QChar(0x00A7) },  // Section Sign
    { 0xA8, QChar(0x00A8) },  // Diaeresis
    { 0xA9, QChar(0x00A9) },  // Copyright Sign
    { 0xAB, QChar(0x00AB) },  // Left-Pointing Double Angle Quotation Mark
    { 0xAC, QChar(0x00AC) },  // Not Sign
    { 0xAD, QChar(0x00AD) },  // Soft Hyphen
    { 0xAF, QChar(0x2015) },  // Horizontal Bar
    { 0xB0, QChar(0x00B0) },  // Degree Sign
    { 0xB1, QChar(0x00B1) },  // Plus-Minus Sign
    { 0xB2, QChar(0x00B2) },  // Superscript Two
    { 0xB3, QChar(0x00B3) },  // Superscript Three
    { 0xB4, QChar(0x0384) },  // Greek Tonos
    { 0xB5, QChar(0x0385) },  // Diaeresis with Tonos
    { 0xB6, QChar(0x0386) },  // Greek Capital Letter Alpha with Tonos
    { 0xB7, QChar(0x00B7) },  // Middle Dot
    { 0xB8, QChar(0x0388) },  // Greek Capital Letter Epsilon with Tonos
    { 0xB9, QChar(0x0389) },  // Greek Capital Letter Eta with Tonos
    { 0xBA, QChar(0x038A) },  // Greek Capital Letter Iota with Tonos
    { 0xBB, QChar(0x00BB) },  // Right-Pointing Double Angle Quotation Mark
    { 0xBC, QChar(0x038C) },  // Greek Capital Letter Omicron with Tonos
    { 0xBD, QChar(0x00BD) },  // Vulgar Fraction One Half
    { 0xBE, QChar(0x038E) },  // Greek Capital Letter Upsilon with Tonos
    { 0xBF, QChar(0x038F) },  // Greek Capital Letter Omega with Tonos
    { 0xC0, QChar(0x0390) },  // Greek Small Letter Iota with Dialytika and Tonos
    { 0xC1, QChar(0x0391) },  // Greek Capital Letter Alpha
    { 0xC2, QChar(0x0392) },  // Greek Capital Letter Beta
    { 0xC3, QChar(0x0393) },  // Greek Capital Letter Gamma
    { 0xC4, QChar(0x0394) },  // Greek Capital Letter Delta
    { 0xC5, QChar(0x0395) },  // Greek Capital Letter Epsilon
};

// Decode ISO-8859-7 manually
QString Interpret_As_ISO_8859_7::decodeISO88597(const QByteArray& iso88597Data) {
    QString result;

    for (char byte : iso88597Data) {
        uint8_t isoChar = static_cast<uint8_t>(byte);

        if (isoChar < 0x80) {
            // ASCII range (direct mapping)
            result.append(QChar(isoChar));
        } else {
            // Map ISO-8859-7 characters, fallback to replacement char if not found
            result.append(iso88597ToUnicode.value(isoChar, QChar(isoChar)));
        }
    }

    return result;
}

// Execute ISO-8859-7 Interpretation
void Interpret_As_ISO_8859_7::execute(QPlainTextEdit* editor) {
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

    QByteArray iso88597Data = file.readAll();
    file.close();

    QString decodedText = decodeISO88597(iso88597Data);
    editor->setPlainText(decodedText);

    qDebug() << "[DEBUG] ISO-8859-7 Decoding Applied to File:" << filePath;
}
