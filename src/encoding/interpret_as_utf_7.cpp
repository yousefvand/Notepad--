#include "interpret_as_utf_7.h"
#include "../codeeditor.h"
#include <QFile>
#include <QDebug>
#include <QPlainTextEdit>

// Singleton instance
Interpret_As_Utf_7& Interpret_As_Utf_7::instance() {
    static Interpret_As_Utf_7 instance;
    return instance;
}

// Helper to decode Base64 for UTF-7 sections
QByteArray decodeBase64SectionUtf7(const QByteArray& input) {
    static const char base64Table[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    QByteArray output;
    int buffer = 0;
    int bitsCollected = 0;

    for (char ch : input) {
        if (ch == '-') {
            break;  // End of shift section
        }

        int value = strchr(base64Table, ch) - base64Table;
        if (value < 0) {
            continue;  // Ignore invalid characters
        }

        buffer = (buffer << 6) | value;
        bitsCollected += 6;

        // Extract 16-bit units (UTF-16 codepoints)
        while (bitsCollected >= 16) {
            bitsCollected -= 16;
            output.append(static_cast<char>((buffer >> bitsCollected) & 0xFF));      // Low byte
            output.append(static_cast<char>((buffer >> (bitsCollected + 8)) & 0xFF)); // High byte
        }
    }

    // Handle leftover bits (padding)
    if (bitsCollected > 0) {
        buffer <<= (16 - bitsCollected);  // Pad to 16 bits
        output.append(static_cast<char>((buffer >> 8) & 0xFF));  // Low byte
        output.append(static_cast<char>(buffer & 0xFF));         // High byte
    }

    return output;
}

// Decode UTF-7 Byte Array to QString (Manual Decoder)
QString Interpret_As_Utf_7::decodeUTF7(const QByteArray& utf7Data) {
    QString result;
    QByteArray buffer;
    bool inShift = false;

    for (char byte : utf7Data) {
        if (byte == '+') {
            // Start of Base64 section
            inShift = true;
            if (!buffer.isEmpty()) {
                result.append(QString::fromLatin1(buffer));
                buffer.clear();
            }
        } else if (inShift) {
            if (byte == '-') {
                // End of Base64 section
                QByteArray decoded = decodeBase64SectionUtf7(buffer);
                result.append(QString::fromUtf16(reinterpret_cast<const char16_t*>(decoded.constData()), decoded.size() / 2));
                buffer.clear();
                inShift = false;
            } else {
                buffer.append(byte);
            }
        } else {
            // Direct ASCII passthrough
            result.append(QChar(byte));
        }
    }

    // Handle remaining ASCII buffer
    if (!buffer.isEmpty()) {
        result.append(QString::fromLatin1(buffer));
    }

    return result;
}

// Execute UTF-7 Interpretation
void Interpret_As_Utf_7::execute(QPlainTextEdit* editor) {
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

    QByteArray utf7Data = file.readAll();
    file.close();

    qDebug() << "[DEBUG] Raw File Data (Hex):" << utf7Data.toHex();

    QString decodedText = decodeUTF7(utf7Data);

    codeEditor->setPlainText(decodedText);
    qDebug() << "[DEBUG] Successfully decoded as UTF-7:" << filePath;
}
