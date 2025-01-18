#include "../codeeditor.h"
#include "interpret_as_cesu_8.h"
#include <QFile>
#include <QDebug>
#include <QStringDecoder>

// Singleton instance
Interpret_As_CESU_8& Interpret_As_CESU_8::instance() {
    static Interpret_As_CESU_8 instance;
    return instance;
}

// Constructor
Interpret_As_CESU_8::Interpret_As_CESU_8() {}

// Main execution function
void Interpret_As_CESU_8::execute(QPlainTextEdit* editor) {
    if (!editor) {
        qWarning() << "[ERROR] No editor instance provided.";
        return;
    }

    CodeEditor* codeEditor = qobject_cast<CodeEditor*>(editor);
    if (!codeEditor) {
        qWarning() << "[ERROR] Invalid CodeEditor instance.";
        return;
    }

    QString filePath = codeEditor->filePath();
    if (filePath.isEmpty()) {
        qWarning() << "[ERROR] No file path associated with the editor.";
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "[ERROR] Cannot open file:" << filePath;
        return;
    }

    QByteArray rawData = file.readAll();
    file.close();

    QString decodedText = decodeCESU8(rawData);
    editor->setPlainText(decodedText);
    qDebug() << "[DEBUG] CESU-8 Decoding applied for file:" << filePath;
}

// Decode CESU-8 encoded data
QString Interpret_As_CESU_8::decodeCESU8(const QByteArray& rawData) {
    QString result;
    int i = 0;

    while (i < rawData.size()) {
        unsigned char byte1 = rawData[i] & 0xFF;

        // ASCII range (single-byte)
        if (byte1 < 0x80) {
            result.append(QChar(byte1));
            i++;
        }
        // CESU-8: 3-byte sequence (surrogate pair high or low)
        else if (byte1 >= 0xE0 && byte1 <= 0xEF) {
            if (i + 2 < rawData.size()) {
                unsigned char byte2 = rawData[i + 1] & 0xFF;
                unsigned char byte3 = rawData[i + 2] & 0xFF;

                if ((byte2 & 0xC0) == 0x80 && (byte3 & 0xC0) == 0x80) {
                    ushort codeUnit = ((byte1 & 0x0F) << 12) |
                                      ((byte2 & 0x3F) << 6) |
                                      (byte3 & 0x3F);

                    if (codeUnit >= 0xD800 && codeUnit <= 0xDBFF) { // High surrogate
                        if (i + 5 < rawData.size()) {
                            unsigned char byte4 = rawData[i + 3] & 0xFF;
                            unsigned char byte5 = rawData[i + 4] & 0xFF;
                            unsigned char byte6 = rawData[i + 5] & 0xFF;

                            if ((byte4 & 0xE0) == 0xE0 && (byte5 & 0xC0) == 0x80 && (byte6 & 0xC0) == 0x80) {
                                ushort lowSurrogate = ((byte4 & 0x0F) << 12) |
                                                      ((byte5 & 0x3F) << 6) |
                                                      (byte6 & 0x3F);

                                if (lowSurrogate >= 0xDC00 && lowSurrogate <= 0xDFFF) {
                                    // Combine surrogates to form a Unicode code point
                                    uint codePoint = 0x10000 + ((codeUnit - 0xD800) << 10) + (lowSurrogate - 0xDC00);
                                    result.append(QChar::fromUcs4(codePoint));
                                    i += 6; // Skip both surrogates
                                    continue;
                                }
                            }
                        }
                    }

                    // Append high surrogate if no matching low surrogate
                    result.append(QChar(codeUnit));
                    i += 3;
                } else {
                    result.append(QChar(0xFFFD)); // Invalid 3-byte sequence
                    i++;
                }
            } else {
                result.append(QChar(0xFFFD)); // Incomplete 3-byte sequence
                i++;
            }
        }
        // Invalid or unexpected byte
        else {
            result.append(QChar(0xFFFD));
            i++;
        }
    }

    return result;
}
