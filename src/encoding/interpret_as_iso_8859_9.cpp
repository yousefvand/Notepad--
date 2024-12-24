#include "interpret_as_iso_8859_9.h"
#include <QFile>
#include <QDebug>
#include <QPlainTextEdit>
#include "../codeeditor.h"

// Singleton instance
Interpret_As_ISO_8859_9& Interpret_As_ISO_8859_9::instance() {
    static Interpret_As_ISO_8859_9 instance;
    return instance;
}

// ISO-8859-9 to Unicode mapping table
const QMap<uint8_t, QChar> iso88599ToUnicode = {
    { 0xA3, QChar(0x20A4) },  // Turkish Lira sign (₤)
    { 0xD0, QChar(0x011E) },  // Latin Capital Letter G with Breve (Ğ)
    { 0xDD, QChar(0x0130) },  // Latin Capital Letter I with Dot Above (İ)
    { 0xDE, QChar(0x015E) },  // Latin Capital Letter S with Cedilla (Ş)
    { 0xF0, QChar(0x011F) },  // Latin Small Letter g with Breve (ğ)
    { 0xFD, QChar(0x0131) },  // Latin Small Letter dotless i (ı)
    { 0xFE, QChar(0x015F) },  // Latin Small Letter s with Cedilla (ş)
    { 0xFF, QChar(0x00FF) }   // Latin Small Letter y with Diaeresis (ÿ)
};

// Decode ISO-8859-9 manually
QString Interpret_As_ISO_8859_9::decodeISO88599(const QByteArray& iso88599Data) {
    QString result;

    for (char byte : iso88599Data) {
        uint8_t isoChar = static_cast<uint8_t>(byte);

        if (isoChar < 0x80) {
            // ASCII range (direct mapping)
            result.append(QChar(isoChar));
        } else {
            // Map ISO-8859-9 characters, fallback to replacement char if not found
            result.append(iso88599ToUnicode.value(isoChar, QChar(isoChar)));
        }
    }

    return result;
}

// Execute ISO-8859-9 Interpretation
void Interpret_As_ISO_8859_9::execute(QPlainTextEdit* editor) {
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

    QByteArray iso88599Data = file.readAll();
    file.close();

    QString decodedText = decodeISO88599(iso88599Data);
    editor->setPlainText(decodedText);

    qDebug() << "[DEBUG] ISO-8859-9 Decoding Applied to File:" << filePath;
}
