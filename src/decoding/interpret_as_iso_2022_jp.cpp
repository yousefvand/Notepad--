#include "../codeeditor.h"
#include "interpret_as_iso_2022_jp.h"
#include <QFile>
#include <QDebug>

// Singleton instance
Interpret_As_ISO_2022_JP& Interpret_As_ISO_2022_JP::instance() {
    static Interpret_As_ISO_2022_JP instance;
    return instance;
}

// Extended JIS Table for ISO-2022-JP (JIS X 0208 mapping)
const std::unordered_map<QByteArray, QChar> Interpret_As_ISO_2022_JP::jisTable = {
    {QByteArray("\x24\x21"), QChar(0x3000)},  // Ideographic space
    {QByteArray("\x24\x22"), QChar(0x3001)},  // 。 (period)
    {QByteArray("\x24\x23"), QChar(0x3002)},  // 、 (comma)
    {QByteArray("\x24\x3C"), QChar(0x3093)},  // ん (n)
    {QByteArray("\x24\x35"), QChar(0x3061)},  // ち (chi)
    {QByteArray("\x24\x25"), QChar(0x4E00)},  // 一 (one)
    {QByteArray("\x24\x26"), QChar(0x4E8C)},  // 二 (two)
    {QByteArray("\x24\x27"), QChar(0x4E09)},  // 三 (three)
    {QByteArray("\x24\x28"), QChar(0x56DB)},  // 四 (four)
    {QByteArray("\x24\x29"), QChar(0x4E94)},  // 五 (five)
    {QByteArray("\x24\x2A"), QChar(0x516D)},  // 六 (six)
    {QByteArray("\x24\x2B"), QChar(0x4E03)},  // 七 (seven)
    {QByteArray("\x24\x2C"), QChar(0x516B)},  // 八 (eight)
    {QByteArray("\x24\x2D"), QChar(0x4E5D)},  // 九 (nine)
    {QByteArray("\x24\x2E"), QChar(0x5341)},  // 十 (ten)
    {QByteArray("\x24\x50"), QChar(0x65E5)},  // 日 (sun, day)
    {QByteArray("\x24\x51"), QChar(0x6708)},  // 月 (moon, month)
    {QByteArray("\x24\x52"), QChar(0x706B)},  // 火 (fire, Tuesday)
    {QByteArray("\x24\x53"), QChar(0x6C34)},  // 水 (water, Wednesday)
    {QByteArray("\x24\x54"), QChar(0x6728)},  // 木 (tree, Thursday)
    {QByteArray("\x24\x55"), QChar(0x91D1)},  // 金 (gold, Friday)
    {QByteArray("\x24\x56"), QChar(0x571F)},  // 土 (earth, Saturday)
    {QByteArray("\x24\x30"), QChar(0x3042)},  // あ (a)
    {QByteArray("\x24\x31"), QChar(0x3044)},  // い (i)
    {QByteArray("\x24\x32"), QChar(0x306B)},  // に (ni)
    {QByteArray("\x24\x33"), QChar(0x3053)},  // こ (ko)
};

// Constructor
Interpret_As_ISO_2022_JP::Interpret_As_ISO_2022_JP() {}

// Main execution function
void Interpret_As_ISO_2022_JP::execute(QPlainTextEdit* editor) {
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

    QString decodedText = decodeISO2022JP(rawData);
    editor->setPlainText(decodedText);
    qDebug() << "[DEBUG] ISO-2022-JP Decoding applied for file:" << filePath;
}

// Decode raw data from ISO-2022-JP
QString Interpret_As_ISO_2022_JP::decodeISO2022JP(const QByteArray& rawData) {
    QString result;
    QByteArray buffer;
    bool inKanjiMode = false;

    for (int i = 0; i < rawData.size(); ++i) {
        if (rawData.mid(i, 3) == "\x1B\x24\x42") {
            inKanjiMode = true;
            i += 2;
        } else if (rawData.mid(i, 3) == "\x1B\x28\x42") {
            inKanjiMode = false;
            i += 2;
        } else if (inKanjiMode) {
            buffer.append(rawData[i]);
            if (buffer.size() == 2) {
                result.append(jisTable.contains(buffer) ? jisTable.at(buffer) : QChar(0xFFFD));
                buffer.clear();
            }
        } else {
            result.append(QChar(rawData[i] & 0xFF));
        }
    }
    return result;
}
