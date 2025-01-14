#include "../codeeditor.h"
#include "interpret_as_iso_2022_jp_2.h"
#include <QFile>
#include <QDebug>

// Singleton instance
Interpret_As_ISO_2022_JP_2& Interpret_As_ISO_2022_JP_2::instance() {
    static Interpret_As_ISO_2022_JP_2 instance;
    return instance;
}

// Static JIS X 0208 Mapping Table
const std::unordered_map<QByteArray, QChar> Interpret_As_ISO_2022_JP_2::jisTable = {
    {QByteArray("\x24\x33"), QChar(0x3053)}, // こ (ko)
    {QByteArray("\x24\x3C"), QChar(0x3093)}, // ん (n)
    {QByteArray("\x24\x32"), QChar(0x306B)}, // に (ni) - Ensure this exists
    {QByteArray("\x24\x35"), QChar(0x3061)}, // ち (chi)
    {QByteArray("\x24\x37"), QChar(0x306F)}  // は (ha)
};

// Constructor
Interpret_As_ISO_2022_JP_2::Interpret_As_ISO_2022_JP_2() {}

// Main execution method to interpret and set text
void Interpret_As_ISO_2022_JP_2::execute(QPlainTextEdit* editor) {
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

    QString decodedText = decodeISO2022JP2(rawData);
    editor->setPlainText(decodedText);
    qDebug() << "[DEBUG] ISO-2022-JP-2 Decoding applied for file:" << filePath;
}

// Decode ISO-2022-JP-2 raw data to QString
QString Interpret_As_ISO_2022_JP_2::decodeISO2022JP2(const QByteArray& rawData) {
    QString result;
    QByteArray buffer;
    bool inKanjiMode = false;

    for (int i = 0; i < rawData.size(); ++i) {
        if (rawData.mid(i, 3) == "\x1B\x24\x42") {  // Enter Kanji mode (JIS X 0208)
            inKanjiMode = true;
            i += 2;  // Skip escape sequence
        } else if (rawData.mid(i, 3) == "\x1B\x28\x42") {  // Return to ASCII mode
            inKanjiMode = false;
            i += 2;
        } else if (inKanjiMode) {
            buffer.append(rawData[i]);
            if (buffer.size() == 2) {
                if (jisTable.contains(buffer)) {
                    result.append(jisTable.at(buffer));
                } else {
                    result.append(QChar(0xFFFD));  // Replacement character for unmapped bytes
                }
                buffer.clear();  // Reset buffer after processing two bytes
            }
        } else {
            result.append(QChar(rawData[i] & 0xFF));  // ASCII mode
        }
    }
    return result;
}
