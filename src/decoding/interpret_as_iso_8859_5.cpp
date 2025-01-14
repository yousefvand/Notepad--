#include "interpret_as_iso_8859_5.h"
#include <QFile>
#include <QDebug>
#include <QPlainTextEdit>
#include "../codeeditor.h"

// Singleton instance
Interpret_As_ISO_8859_5& Interpret_As_ISO_8859_5::instance() {
    static Interpret_As_ISO_8859_5 instance;
    return instance;
}

// Full ISO-8859-5 to Unicode mapping table
const QMap<uint8_t, QChar> iso88595ToUnicode = {
    { 0xA0, QChar(0x00A0) },  // No-Break Space
    { 0xA1, QChar(0x0401) },  // Ё
    { 0xA2, QChar(0x0402) },  // Ђ
    { 0xA3, QChar(0x0403) },  // Ѓ
    { 0xA4, QChar(0x0404) },  // Є
    { 0xA5, QChar(0x0405) },  // Ѕ
    { 0xA6, QChar(0x0406) },  // І
    { 0xA7, QChar(0x0407) },  // Ї
    { 0xA8, QChar(0x0408) },  // Ј
    { 0xA9, QChar(0x0409) },  // Љ
    { 0xB0, QChar(0x0410) },  // А
    { 0xB1, QChar(0x0411) },  // Б
    { 0xB2, QChar(0x0412) },  // В
    { 0xB3, QChar(0x0413) },  // Г
    { 0xB4, QChar(0x0414) },  // Д
    { 0xB5, QChar(0x0415) },  // Е
    { 0xB6, QChar(0x0416) },  // Ж
    { 0xB7, QChar(0x0417) },  // З
    { 0xB8, QChar(0x0418) },  // И
    { 0xB9, QChar(0x0419) },  // Й
    { 0xBA, QChar(0x041A) },  // К
    { 0xBB, QChar(0x041B) },  // Л
    { 0xBC, QChar(0x041C) },  // М
    { 0xBD, QChar(0x041D) },  // Н
    { 0xBE, QChar(0x041E) },  // О
    { 0xBF, QChar(0x041F) },  // П
    { 0xC0, QChar(0x0420) },  // Р
    { 0xC1, QChar(0x0421) },  // С
};

// Manual ISO-8859-5 Decoder
QString Interpret_As_ISO_8859_5::decodeISO88595(const QByteArray& iso88595Data) {
    QString result;
    for (char byte : iso88595Data) {
        uint8_t isoChar = static_cast<uint8_t>(byte);
        result.append(iso88595ToUnicode.value(isoChar, QChar(isoChar)));
    }
    return result;
}

// Execute and Force Decode
void Interpret_As_ISO_8859_5::execute(QPlainTextEdit* editor) {
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

    QByteArray rawData = file.readAll();
    file.close();

    qDebug() << "[DEBUG] Raw Bytes (Hex):" << rawData.toHex();

    // Force Manual Interpretation (Bypass QTextEdit Encoding)
    QString decodedText = decodeISO88595(rawData);
    editor->setPlainText(decodedText);

    qDebug() << "[DEBUG] Decoding applied for file:" << filePath;
}
