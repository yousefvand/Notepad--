#include "../codeeditor.h"
#include "interpret_as_iso_2022_cn_ext.h"
#include <QFile>
#include <QDebug>

// Singleton instance
Interpret_As_ISO_2022_CN_EXT& Interpret_As_ISO_2022_CN_EXT::instance() {
    static Interpret_As_ISO_2022_CN_EXT instance;
    return instance;
}

// GB Table for ISO-2022-CN-EXT (Limited set of GB2312 mappings)
const std::unordered_map<QByteArray, QChar> Interpret_As_ISO_2022_CN_EXT::gbTable = {
    {QByteArray("\x21\x21"), QChar(0x3000)},  // Ideographic space
    {QByteArray("\x21\x22"), QChar(0x3001)},  // 。 (period)
    {QByteArray("\x21\x23"), QChar(0x3002)},  // 、 (comma)
    {QByteArray("\x21\x27"), QChar(0x4E00)},  // 一 (one)
    {QByteArray("\x21\x28"), QChar(0x4E8C)},  // 二 (two)
    {QByteArray("\x21\x29"), QChar(0x4E09)},  // 三 (three)
    {QByteArray("\x21\x2A"), QChar(0x56DB)},  // 四 (four)
    {QByteArray("\x21\x2B"), QChar(0x4E94)},  // 五 (five)
    {QByteArray("\x21\x2C"), QChar(0x516D)},  // 六 (six)
    {QByteArray("\x21\x2D"), QChar(0x4E03)},  // 七 (seven)
    {QByteArray("\x21\x2E"), QChar(0x516B)},  // 八 (eight)
    {QByteArray("\x21\x2F"), QChar(0x4E5D)},  // 九 (nine)
    {QByteArray("\x21\x30"), QChar(0x5341)},  // 十 (ten)
    {QByteArray("\x21\x40"), QChar(0x65E5)},  // 日 (day)
    {QByteArray("\x21\x41"), QChar(0x6708)},  // 月 (moon)
    {QByteArray("\x21\x42"), QChar(0x706B)},  // 火 (fire)
    {QByteArray("\x21\x43"), QChar(0x6C34)},  // 水 (water)
};

// Constructor
Interpret_As_ISO_2022_CN_EXT::Interpret_As_ISO_2022_CN_EXT() {}

// Main execution function
void Interpret_As_ISO_2022_CN_EXT::execute(QPlainTextEdit* editor) {
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

    QString decodedText = decodeISO2022CNEXT(rawData);
    editor->setPlainText(decodedText);
    qDebug() << "[DEBUG] ISO-2022-CN-EXT Decoding applied for file:" << filePath;
}

// Decode raw data from ISO-2022-CN-EXT encoding
QString Interpret_As_ISO_2022_CN_EXT::decodeISO2022CNEXT(const QByteArray& rawData) {
    QString result;
    QByteArray buffer;
    bool inGBMode = false;

    for (int i = 0; i < rawData.size(); ++i) {
        if (rawData.mid(i, 4) == "\x1B\x24\x29\x41") {  // Enter GB2312 mode
            inGBMode = true;
            i += 3;
        } else if (rawData.mid(i, 3) == "\x1B\x28\x42") {  // Return to ASCII
            inGBMode = false;
            i += 2;
        } else if (inGBMode) {
            buffer.append(rawData[i]);
            if (buffer.size() == 2) {
                if (gbTable.contains(buffer)) {
                    result.append(gbTable.at(buffer));
                } else {
                    result.append(QChar(0xFFFD));  // Fallback for unmapped bytes
                }
                buffer.clear();
            }
        } else {
            result.append(QChar(rawData[i] & 0xFF));  // ASCII Mode
        }
    }
    return result;
}
