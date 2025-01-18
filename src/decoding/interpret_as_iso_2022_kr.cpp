#include "interpret_as_iso_2022_kr.h"
#include <QFile>
#include <QDebug>
#include <QPlainTextEdit>
#include "../codeeditor.h"

// Singleton instance
Interpret_As_ISO_2022_KR& Interpret_As_ISO_2022_KR::instance() {
    static Interpret_As_ISO_2022_KR instance;
    return instance;
}

// Decode ISO-2022-KR manually
QString Interpret_As_ISO_2022_KR::decodeISO2022KR(const QByteArray& iso2022krData) {
    QString result;
    bool koreanMode = false;

    for (int i = 0; i < iso2022krData.size(); ++i) {
        unsigned char byte = static_cast<unsigned char>(iso2022krData[i]);

        // Handle Escape Sequences
        if (byte == 0x1B) {  // ESC (Start of escape sequence)
            if (i + 3 < iso2022krData.size() &&
                iso2022krData[i + 1] == '$' &&
                iso2022krData[i + 2] == ')' &&
                iso2022krData[i + 3] == 'C') {

                koreanMode = true;
                i += 3;  // Skip the escape sequence (4 bytes)
                continue;
            }
            if (i + 2 < iso2022krData.size() && iso2022krData[i + 1] == '(') {
                koreanMode = false;
                i += 2;  // Skip ASCII escape sequence
                continue;
            }
        }

        // Shift In (0x0E) - Enable Korean mode
        if (byte == 0x0E) {
            koreanMode = true;
            continue;
        }
        // Shift Out (0x0F) - Disable Korean mode
        else if (byte == 0x0F) {
            koreanMode = false;
            continue;
        }

        // Korean mode processing (two-byte pairs)
        if (koreanMode && i + 1 < iso2022krData.size()) {
            unsigned char nextByte = static_cast<unsigned char>(iso2022krData[i + 1]);
            int codePoint = (byte << 8) | nextByte;

            if (codePoint >= 0xB0A1 && codePoint <= 0xC8FE) {
                int unicodeHangul = 0xAC00 + ((codePoint - 0xB0A1) / 94) * 28 + ((codePoint - 0xB0A1) % 94);
                result.append(QChar(unicodeHangul));
            } else {
                result.append(QChar('?'));  // Fallback for unmapped Korean characters
            }
            i++;  // Skip the next byte (part of the pair)
        } else if (!koreanMode) {
            // ASCII Mode - Append directly
            result.append(QChar(byte));
        }
    }

    return result;
}

// Execute Interpretation
void Interpret_As_ISO_2022_KR::execute(QPlainTextEdit* editor) {
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

    QByteArray iso2022krData = file.readAll();
    file.close();

    qDebug() << "[DEBUG] Raw Bytes (Hex):" << iso2022krData.toHex();

    QString decodedText = decodeISO2022KR(iso2022krData);
    editor->setPlainText(decodedText);

    qDebug() << "[DEBUG] ISO-2022-KR Decoding applied for file:" << filePath;
}
