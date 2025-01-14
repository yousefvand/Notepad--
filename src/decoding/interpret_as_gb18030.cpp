#include "../codeeditor.h"
#include "interpret_as_gb18030.h"
#include <QFile>
#include <QDebug>
#include <QStringDecoder>

// Singleton instance
Interpret_As_GB18030& Interpret_As_GB18030::instance() {
    static Interpret_As_GB18030 instance;
    return instance;
}

// Constructor
Interpret_As_GB18030::Interpret_As_GB18030() {}

// Main execution function
void Interpret_As_GB18030::execute(QPlainTextEdit* editor) {
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

    QString decodedText = decodeGB18030(rawData);
    editor->setPlainText(decodedText);
    qDebug() << "[DEBUG] GB18030 Decoding applied for file:" << filePath;
}

// Decode GB18030 encoded data
QString Interpret_As_GB18030::decodeGB18030(const QByteArray& rawData) {
    QStringDecoder decoder("GB18030");

    if (!decoder.isValid()) {
        qWarning() << "[ERROR] GB18030 decoder is invalid.";
        return QString();
    }

    QString result;
    QByteArray buffer;

    for (int i = 0; i < rawData.size(); ++i) {
        unsigned char byte = rawData[i];

        // ASCII range (single-byte)
        if (byte < 0x80) {
            if (!buffer.isEmpty()) {
                // Decode any pending multi-byte buffer
                result.append(decoder.decode(buffer));
                buffer.clear();
            }
            result.append(QChar(byte));  // Directly append ASCII
        }
        // Start of multi-byte sequence
        else {
            buffer.append(byte);

            // Decode buffer if it's a valid GB18030 multi-byte sequence
            if (buffer.size() == 2 || buffer.size() == 4) {
                QString decoded = decoder.decode(buffer);
                if (!decoded.isEmpty()) {
                    result.append(decoded);
                    buffer.clear();
                }
            }
        }
    }

    // Process any remaining buffer (ensure valid decoding or append replacement)
    if (!buffer.isEmpty()) {
        QString remainingDecoded = decoder.decode(buffer);
        if (!remainingDecoded.isEmpty()) {
            result.append(remainingDecoded);
        } else {
            result.append(QString(buffer.size(), QChar(0xFFFD)));  // Replace each invalid byte with �
        }
    }

    return result;
}
