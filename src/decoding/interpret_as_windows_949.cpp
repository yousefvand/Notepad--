#include "../codeeditor.h"
#include "interpret_as_windows_949.h"
#include <QFile>
#include <QDebug>

// Singleton instance
Interpret_As_windows_949& Interpret_As_windows_949::instance() {
    static Interpret_As_windows_949 instance;
    return instance;
}

// Constructor
Interpret_As_windows_949::Interpret_As_windows_949() {}

// Main execution function
void Interpret_As_windows_949::execute(QPlainTextEdit* editor) {
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

    QString decodedText = decodeWindows949(rawData);
    editor->setPlainText(decodedText);
    qDebug() << "[DEBUG] Windows-949 decoding applied for file:" << filePath;
}

// Decode Windows-949 encoded data
QString Interpret_As_windows_949::decodeWindows949(const QByteArray& rawData) {
    QString result;
    QByteArray buffer;

    for (int i = 0; i < rawData.size(); ++i) {
        unsigned char byte = rawData[i];

        // Detect double-byte characters
        if (byte >= 0x81 && byte <= 0xFE) {
            buffer.append(byte);
            if (buffer.size() == 2) {
                auto it = windows949Table.find(buffer);
                if (it != windows949Table.end()) {
                    result.append(it->second);
                } else {
                    result.append(QChar(0xFFFD)); // Fallback for unmapped bytes
                }
                buffer.clear();
            }
        } else {
            if (!buffer.isEmpty()) {
                result.append(QChar(0xFFFD)); // Handle incomplete sequences
                buffer.clear();
            }
            result.append(QChar(byte)); // Single-byte character
        }
    }

    return result;
}

// Simplified Windows-949 decoding table (expand as needed)
const std::unordered_map<QByteArray, QChar> Interpret_As_windows_949::windows949Table = {
    {QByteArray("\xB0\xA1"), QChar(0xAC00)}, // 가
    {QByteArray("\xB0\xA2"), QChar(0xAC01)}, // 각
    {QByteArray("\xB0\xA3"), QChar(0xAC04)}, // 간
    {QByteArray("\xB0\xA4"), QChar(0xAC07)}, // 갈
    {QByteArray("\xB0\xA5"), QChar(0xAC08)}, // 감
    {QByteArray("\xB0\xA6"), QChar(0xAC09)}, // 갑
    {QByteArray("\xB0\xA7"), QChar(0xAC0A)}, // 값
    {QByteArray("\xB0\xA8"), QChar(0xAC10)}, // 강
    {QByteArray("\xB0\xA9"), QChar(0xAC11)}, // 갓
    {QByteArray("\xB0\xAA"), QChar(0xAC12)}, // 같
    {QByteArray("\xB0\xAB"), QChar(0xAC13)}, // 갔
    {QByteArray("\xB0\xAC"), QChar(0xAC14)}, // 강
    {QByteArray("\xB0\xAD"), QChar(0xAC15)}, // 강
    // ... Add more mappings as needed
};
