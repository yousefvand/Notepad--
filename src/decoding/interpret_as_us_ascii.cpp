#include "interpret_as_us_ascii.h"
#include <QString>
#include <QDebug>
#include <QPlainTextEdit>
#include <QFile>

Interpret_As_US_ASCII& Interpret_As_US_ASCII::instance() {
    static Interpret_As_US_ASCII instance;
    return instance;
}

Interpret_As_US_ASCII::Interpret_As_US_ASCII() = default;
Interpret_As_US_ASCII::~Interpret_As_US_ASCII() = default;

void Interpret_As_US_ASCII::execute(QPlainTextEdit* editor) {
    if (!editor) {
        qDebug() << "[ERROR] No QPlainTextEdit instance provided.";
        return;
    }

    // Assume the document is already loaded in the editor
    QString filePath = editor->property("filePath").toString(); // Adjust if you store filePath differently
    if (filePath.isEmpty()) {
        qDebug() << "[ERROR] No file path associated with the editor.";
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "[ERROR] Failed to open file:" << filePath;
        return;
    }

    QByteArray fileContent = file.readAll();
    file.close();

    qDebug() << "[DEBUG] Raw file content (Hex):" << fileContent.toHex();

    QString decodedText = decodeUsAscii(fileContent);
    if (decodedText.isEmpty()) {
        qDebug() << "[ERROR] Decoding US-ASCII content failed.";
        return;
    }

    editor->setPlainText(decodedText);
    qDebug() << "[DEBUG] Updated editor content with decoded US-ASCII text.";
}

QString Interpret_As_US_ASCII::decodeUsAscii(const QByteArray& input) {
    QString result;
    for (char c : input) { // Process as `char`
        if (static_cast<unsigned char>(c) <= 127) { // Explicit cast to avoid warnings
            result += QChar(c); // Convert to QChar
        } else {
            qWarning() << "[WARNING] Non-ASCII character encountered:" << static_cast<int>(c);
            result += QChar('?'); // Replace non-ASCII characters with a placeholder
        }
    }
    return result;
}
