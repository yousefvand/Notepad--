#include "interpret_as_utf_32_le.h"
#include "../codeeditor.h"
#include <QDebug>
#include <QPlainTextEdit>
#include <QFile>
#include <QStringDecoder>
#include <QTextStream>
#include <QFont>
#include <QStringConverter>

// Singleton instance
Interpret_As_UTF_32_LE& Interpret_As_UTF_32_LE::instance() {
    static Interpret_As_UTF_32_LE instance;
    return instance;
}

// Decode UTF-32LE Byte Array to QString
QString Interpret_As_UTF_32_LE::decodeUTF32LE(const QByteArray& utf32Data) {
    if (utf32Data.size() % 4 != 0) {
        qWarning() << "[ERROR] Invalid UTF-32LE byte length. Corrupt file?";
        return QString();
    }

    QString result;
    const char* data = utf32Data.constData();
    int length = utf32Data.size() / 4;

    for (int i = 0; i < length; ++i) {
        uint32_t codepoint = static_cast<uint8_t>(data[i * 4])       |
                             (static_cast<uint8_t>(data[i * 4 + 1]) << 8)  |
                             (static_cast<uint8_t>(data[i * 4 + 2]) << 16) |
                             (static_cast<uint8_t>(data[i * 4 + 3]) << 24);

        if (codepoint <= 0x10FFFF) {
            result.append(QChar::fromUcs4(codepoint));
        } else {
            result.append(QChar(0xFFFD));  // Replacement character (�)
        }
    }

    return result;
}

// Execute UTF-32LE Interpretation
void Interpret_As_UTF_32_LE::execute(QPlainTextEdit* editor) {
    if (!editor) {
        qWarning() << "[ERROR] No editor instance provided.";
        return;
    }

    // Get the file path from the editor
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

    // Read the file as a byte array
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "[ERROR] Cannot open file: " << filePath;
        return;
    }

    QByteArray utf32Data = file.readAll();
    file.close();

    qDebug() << "[DEBUG] Raw File Data (Hex):" << utf32Data.toHex();

    // Decode the UTF-32LE data
    QString decodedText = decodeUTF32LE(utf32Data);

    // Set the decoded text in the editor
    codeEditor->setPlainText(decodedText);

    qDebug() << "[DEBUG] Successfully decoded as UTF-32LE:" << filePath;
}
