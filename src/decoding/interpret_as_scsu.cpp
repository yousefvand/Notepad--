#include "interpret_as_scsu.h"
#include <QTextCursor>
#include <QTextDocument>
#include <QDebug>

Interpret_As_SCSU& Interpret_As_SCSU::instance() {
    static Interpret_As_SCSU instance;
    return instance;
}

void Interpret_As_SCSU::execute(QPlainTextEdit* editor) {
    if (!editor) {
        qWarning() << "[SCSU] No editor instance provided!";
        return;
    }

    QString content = editor->toPlainText();
    QByteArray scsuData = content.toUtf8();

    QString decodedText = decodeSCSU(scsuData);
    if (!decodedText.isEmpty()) {
        editor->setPlainText(decodedText);
        qDebug() << "[SCSU] Decoding complete!";
    } else {
        qWarning() << "[SCSU] Failed to decode text!";
    }
}

QString Interpret_As_SCSU::decodeSCSU(const QByteArray& scsuData) {
    QString result;
    QString decompressedText;
    const unsigned char* data = reinterpret_cast<const unsigned char*>(scsuData.constData());
    size_t length = scsuData.size();

    // State variables for the SCSU decoding
    bool inSingleByteMode = true;  // Start in single-byte mode
    unsigned short windowOffset = 0xE000;  // Default dynamic window offset

    for (size_t i = 0; i < length; ++i) {
        unsigned char byte = data[i];

        if (inSingleByteMode) {
            if (byte < 0x80) {
                // ASCII character
                decompressedText.append(QChar(byte));
            } else if (byte >= 0x80 && byte <= 0x9F) {
                // Reserved control codes (unsupported in this example)
                decompressedText.append(QChar('?'));
            } else if (byte == 0x0E) {
                // Switch to Unicode mode
                inSingleByteMode = false;
            } else {
                // Map to the dynamic window
                decompressedText.append(QChar(windowOffset + byte - 0xA0));
            }
        } else {
            // Unicode mode: decode as UTF-16
            if (i + 1 < length) {
                unsigned short highByte = byte;
                unsigned short lowByte = data[++i];
                unsigned short unicodeChar = (highByte << 8) | lowByte;
                decompressedText.append(QChar(unicodeChar));
            } else {
                qWarning() << "[SCSU] Incomplete Unicode sequence.";
                break;
            }
        }
    }

    return decompressedText;
}
