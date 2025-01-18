#include "../codeeditor.h"
#include "interpret_as_hz_gb_2312.h"
#include <QFile>
#include <QDebug>
#include <QStringDecoder>

// Singleton instance
Interpret_As_HZ_GB_2312& Interpret_As_HZ_GB_2312::instance() {
    static Interpret_As_HZ_GB_2312 instance;
    return instance;
}

// Constructor
Interpret_As_HZ_GB_2312::Interpret_As_HZ_GB_2312() {}

// Main execution function
void Interpret_As_HZ_GB_2312::execute(QPlainTextEdit* editor) {
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

    QString decodedText = decodeHZGB2312(rawData);
    editor->setPlainText(decodedText);
    qDebug() << "[DEBUG] HZ-GB-2312 Decoding applied for file:" << filePath;
}

// Decode HZ-GB-2312 encoded data
QString Interpret_As_HZ_GB_2312::decodeHZGB2312(const QByteArray& rawData) {
    QString result;
    QByteArray buffer;
    bool inGB2312Mode = false;

    QStringDecoder decoder("GB2312");

    if (!decoder.isValid()) {
        qWarning() << "[ERROR] GB2312 decoder is invalid.";
        return QString();
    }

    for (int i = 0; i < rawData.size(); ++i) {
        if (rawData.mid(i, 2) == "~{") {  // Enter GB2312 mode
            inGB2312Mode = true;
            i += 1;  // Skip the "~{"
        } else if (rawData.mid(i, 2) == "~}") {  // Exit GB2312 mode
            if (!buffer.isEmpty()) {
                result.append(decoder.decode(buffer));  // Process any remaining buffer
                buffer.clear();
            }
            inGB2312Mode = false;
            i += 1;  // Skip the "~}"
        } else if (inGB2312Mode) {
            buffer.append(rawData[i]);
            if (buffer.size() == 2) {  // Each GB2312 character is 2 bytes
                result.append(decoder.decode(buffer));
                buffer.clear();
            }
        } else {
            result.append(QChar(rawData[i] & 0xFF));  // ASCII mode
        }
    }

    // Process any remaining buffer in GB2312 mode
    if (inGB2312Mode && !buffer.isEmpty()) {
        result.append(decoder.decode(buffer));
    }

    return result;
}
