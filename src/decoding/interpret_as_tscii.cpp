#include <QStringDecoder>
#include <QFile>
#include <QDebug>
#include "interpret_as_tscii.h"
#include "../codeeditor.h"

const QMap<uint8_t, QChar> Interpret_As_TSCII::tsciiToUnicode = {
    { 0x80, QChar(0x0B85) },  // Tamil Letter A
    { 0x81, QChar(0x0B86) },  // Tamil Letter AA
    { 0x82, QChar(0x0B87) },  // Tamil Letter I
    { 0x83, QChar(0x0B88) },  // Tamil Letter II
    { 0x84, QChar(0x0B89) },  // Tamil Letter U
    { 0x85, QChar(0x0B8A) },  // Tamil Letter UU
    { 0x86, QChar(0x0B8E) },  // Tamil Letter E
    { 0x87, QChar(0x0B8F) },  // Tamil Letter EE
    { 0x88, QChar(0x0B90) },  // Tamil Letter AI
    { 0x89, QChar(0x0B92) },  // Tamil Letter O
    { 0x8A, QChar(0x0B93) },  // Tamil Letter OO
    { 0x8B, QChar(0x0B94) },  // Tamil Letter AU
    { 0x8C, QChar(0x0B95) },  // Tamil Letter KA
    { 0x8D, QChar(0x0B99) },  // Tamil Letter NGA
    { 0x8E, QChar(0x0B9A) },  // Tamil Letter CA
    { 0x8F, QChar(0x0B9E) },  // Tamil Letter NYA
    { 0x90, QChar(0x0B9F) },  // Tamil Letter TTA
    { 0x91, QChar(0x0BA3) },  // Tamil Letter NNA
    { 0x92, QChar(0x0BA4) },  // Tamil Letter TA
    { 0x93, QChar(0x0BA8) },  // Tamil Letter NA
    { 0x94, QChar(0x0BAA) },  // Tamil Letter PA
    { 0x95, QChar(0x0BAE) },  // Tamil Letter MA
    { 0x96, QChar(0x0BAF) },  // Tamil Letter YA
    { 0x97, QChar(0x0BB0) },  // Tamil Letter RA
    { 0x98, QChar(0x0BB2) },  // Tamil Letter LA
    { 0x99, QChar(0x0BB5) },  // Tamil Letter VA
    { 0x9A, QChar(0x0BB4) },  // Tamil Letter LLA
    { 0x9B, QChar(0x0BB3) },  // Tamil Letter LLA (variant)
    { 0x9C, QChar(0x0BB1) },  // Tamil Letter RRA
    { 0x9D, QChar(0x0BA9) },  // Tamil Letter NNNA
    { 0x9E, QChar(0x0BB8) },  // Tamil Letter SA
    { 0x9F, QChar(0x0BB7) },  // Tamil Letter SHA
    { 0xA0, QChar(0x0BB9) },  // Tamil Letter HA
};

// Singleton instance
Interpret_As_TSCII& Interpret_As_TSCII::instance() {
    static Interpret_As_TSCII instance;
    return instance;
}

// Decode TSCII to Unicode (with ASCII passthrough)
QString Interpret_As_TSCII::decodeTSCII(const QByteArray& tsciiData) {
    QString result;

    for (char byte : tsciiData) {
        uint8_t tsciiChar = static_cast<uint8_t>(byte);

        // ASCII passthrough for 0x00 to 0x7F
        if (tsciiChar < 0x80) {
            result.append(QChar(tsciiChar));
        }
        // Map TSCII bytes
        else if (tsciiToUnicode.contains(tsciiChar)) {
            result.append(tsciiToUnicode.value(tsciiChar));
        }
        // Fallback for unmapped bytes
        else {
            qDebug() << "[WARNING] Unmapped TSCII Byte:" << QString::number(tsciiChar, 16).toUpper();
            result.append(QChar(0xFFFD));  // Unicode replacement character (�)
        }
    }

    return result;
}

// Execute TSCII Interpretation
void Interpret_As_TSCII::execute(QPlainTextEdit* editor) {
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

    QByteArray tsciiData = file.readAll();
    file.close();

    qDebug() << "[DEBUG] Raw File Data (Hex):" << tsciiData.toHex();

    QString decodedText = decodeTSCII(tsciiData);
    codeEditor->setPlainText(decodedText);

    qDebug() << "[DEBUG] Reloaded and decoded as TSCII:" << filePath;
}
