#include "../codeeditor.h"
#include "interpret_as_bocu_1.h"
#include <QFile>
#include <QDebug>

// Singleton instance
Interpret_As_BOCU_1& Interpret_As_BOCU_1::instance() {
    static Interpret_As_BOCU_1 instance;
    return instance;
}

// Constructor
Interpret_As_BOCU_1::Interpret_As_BOCU_1() {}

// Main execution function
void Interpret_As_BOCU_1::execute(QPlainTextEdit* editor) {
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

    QString decodedText = decodeBOCU1(rawData);
    editor->setPlainText(decodedText);
    qDebug() << "[DEBUG] BOCU-1 Decoding applied for file:" << filePath;
}

// Decode BOCU-1 encoded data
QString Interpret_As_BOCU_1::decodeBOCU1(const QByteArray& rawData) {
    QString result;
    int index = 0;

    while (index < rawData.size()) {
        int codePoint = decodeBOCU1Char(rawData, index);

        if (codePoint == 0) {
            continue; // Skip reset or empty characters
        }

        if (codePoint <= 0xFFFF) {
            // BMP character
            result.append(QChar(codePoint));
        } else {
            // Supplementary character: convert to surrogate pair
            ushort highSurrogate = 0xD800 + ((codePoint - 0x10000) >> 10);
            ushort lowSurrogate = 0xDC00 + ((codePoint - 0x10000) & 0x3FF);

            result.append(QChar(highSurrogate));
            result.append(QChar(lowSurrogate));
        }
    }

    return result;
}

// Decode a single BOCU-1 byte sequence
int Interpret_As_BOCU_1::decodeBOCU1Char(const QByteArray& data, int& index) {
    static const int BOCU1_RESET = 0x40; // Reset state constant
    static int prevState = BOCU1_RESET; // Initialize state to reset value

    if (index >= data.size()) {
        return 0xFFFD; // Replacement character for invalid input
    }

    unsigned char byte = data[index++];

    if (byte == 0xFF) { // BOCU-1 reset signal
        prevState = BOCU1_RESET;
        return 0; // Skip reset signals
    }

    // Decode current byte as an offset from the previous state
    int offset = byte - BOCU1_RESET;
    int codePoint = prevState + offset;

    // Clamp the code point to valid Unicode range
    codePoint = qBound(0, codePoint, 0x10FFFF);

    // Update the state to the decoded code point
    prevState = codePoint;

    return codePoint;
}
