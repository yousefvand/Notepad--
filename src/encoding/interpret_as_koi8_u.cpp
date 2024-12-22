#include "interpret_as_koi8_u.h"
#include "../codeeditor.h"
#include <QFile>
#include <QDebug>

// Singleton instance
Interpret_As_KOI8_U& Interpret_As_KOI8_U::instance() {
    static Interpret_As_KOI8_U instance;
    return instance;
}

// KOI8-U to Unicode mapping
QChar Interpret_As_KOI8_U::koi8uToUnicode(uint8_t koi8uChar) {
    // ASCII range (0x00-0x7F remains unchanged)
    if (koi8uChar < 0x80) {
        return QChar(koi8uChar);
    }

    // Mapping for KOI8-U specific characters (0x80-0xFF)
    static const uint16_t koi8uToUnicodeTable[128] = {
        0x2500, 0x2502, 0x250C, 0x2510, 0x2514, 0x2518, 0x251C, 0x2524, // 0x80-0x87
        0x252C, 0x2534, 0x253C, 0x2580, 0x2584, 0x2588, 0x258C, 0x2590, // 0x88-0x8F
        // Add the remaining KOI8-U mappings (refer to the KOI8-U standard table)
        // ...
        0xFFFD // Placeholder for unmapped characters
    };

    return QChar(koi8uToUnicodeTable[koi8uChar - 0x80]);
}

// Decode the KOI8-U data
QString Interpret_As_KOI8_U::decodeKOI8U(const QByteArray& koi8uData) {
    QString result;

    for (char byte : koi8uData) {
        QChar unicodeChar = koi8uToUnicode(static_cast<uint8_t>(byte));
        result.append(unicodeChar);
    }

    return result;
}

// Execute the KOI8-U interpretation

void Interpret_As_KOI8_U::execute(QPlainTextEdit* editor) {
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
    if (filePath.isEmpty()) {
        qWarning() << "[ERROR] No file path associated with the editor.";
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "[ERROR] Cannot open file: " << filePath;
        return;
    }

    QByteArray koi8uData = file.readAll();
    file.close();

    // Use QStringDecoder to decode KOI8-U
    QStringDecoder decoder("KOI8-U");

    // Decode KOI8-U data to QString
    QString decodedText = decoder.decode(koi8uData);

    // Replace text in the editor with the correctly decoded text
    codeEditor->setPlainText(decodedText);

    qDebug() << "[DEBUG] Successfully reloaded and decoded as KOI8-U:" << filePath;
}
