#include "interpret_as_utf_32.h"
#include <QFile>
#include <QDebug>

InterpretAsUtf32& InterpretAsUtf32::instance() {
    static InterpretAsUtf32 instance;
    return instance;
}

void InterpretAsUtf32::execute(QPlainTextEdit* editor) {
    if (!editor) {
        qDebug() << "[ERROR] No QPlainTextEdit instance provided.";
        return;
    }

    // Read original text from editor
    QString originalText = editor->toPlainText();
    qDebug() << "[DEBUG] Original text:" << originalText;

    // Convert to UTF-32 and interpret
    QFile file("utf32_test.txt");
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "[ERROR] Failed to open file.";
        return;
    }

    QByteArray utf32Data = file.readAll();
    file.close();

    if (utf32Data.size() % 4 != 0) {
        qDebug() << "[ERROR] Invalid UTF-32 data size.";
        return;
    }

    QVector<char16_t> utf16Data;
    for (int i = 0; i < utf32Data.size(); i += 4) {
        // Decode UTF-32 to char16_t
        uint32_t codePoint = *reinterpret_cast<const uint32_t*>(utf32Data.data() + i);
        if (codePoint <= 0xFFFF) {
            utf16Data.append(static_cast<char16_t>(codePoint));
        } else if (codePoint <= 0x10FFFF) {
            codePoint -= 0x10000;
            utf16Data.append(static_cast<char16_t>(0xD800 + (codePoint >> 10))); // High surrogate
            utf16Data.append(static_cast<char16_t>(0xDC00 + (codePoint & 0x3FF))); // Low surrogate
        } else {
            qDebug() << "[ERROR] Invalid code point in UTF-32 data.";
            return;
        }
    }

    // Convert char16_t data to QString
    QString decodedText = QString::fromUtf16(reinterpret_cast<const char16_t*>(utf16Data.constData()), utf16Data.size());

    qDebug() << "[DEBUG] Decoded text as UTF-32:" << decodedText;

    // Set decoded text back into the editor
    if (!decodedText.isEmpty()) {
        editor->setPlainText(decodedText);
        qDebug() << "[DEBUG] Updated editor content with decoded UTF-32 text.";
    } else {
        qDebug() << "[WARNING] Decoded text is empty.";
    }
}
