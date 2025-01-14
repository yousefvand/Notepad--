#include "interpret_as_iso_8859_1.h"
#include <QFile>
#include <QDebug>
#include <QPlainTextEdit>
#include "../codeeditor.h"

// Singleton instance
Interpret_As_ISO_8859_1& Interpret_As_ISO_8859_1::instance() {
    static Interpret_As_ISO_8859_1 instance;
    return instance;
}

// Decode ISO-8859-1 manually (1:1 mapping for 0xA0 to 0xFF)
QString Interpret_As_ISO_8859_1::decodeISO88591(const QByteArray& iso88591Data) {
    QString result;
    for (char byte : iso88591Data) {
        uint8_t isoChar = static_cast<uint8_t>(byte);
        result.append(QChar(isoChar));  // Direct 1:1 mapping
    }
    return result;
}

// Execute Interpretation
void Interpret_As_ISO_8859_1::execute(QPlainTextEdit* editor) {
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

    QByteArray iso88591Data = file.readAll();
    file.close();

    // Debugging: Print Raw Bytes
    qDebug() << "[DEBUG] Raw Bytes (Hex):" << iso88591Data.toHex();

    QString decodedText = decodeISO88591(iso88591Data);
    editor->setPlainText(decodedText);

    qDebug() << "[DEBUG] Decoding applied for file:" << filePath;
}
