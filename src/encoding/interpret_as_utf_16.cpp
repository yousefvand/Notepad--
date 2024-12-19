#include "interpret_as_utf_16.h"
#include <QString>
#include <QDebug>
#include <QFile>
#include <QPlainTextEdit>

Interpret_As_UTF_16& Interpret_As_UTF_16::instance() {
    static Interpret_As_UTF_16 instance;
    return instance;
}

Interpret_As_UTF_16::Interpret_As_UTF_16() = default;
Interpret_As_UTF_16::~Interpret_As_UTF_16() = default;

void Interpret_As_UTF_16::execute(QPlainTextEdit* editor) {
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

    QString decodedText = decodeUtf16(fileContent);
    if (decodedText.isEmpty()) {
        qDebug() << "[ERROR] Decoding UTF-16 content failed.";
        return;
    }

    editor->setPlainText(decodedText);
    qDebug() << "[DEBUG] Updated editor content with decoded UTF-16 text.";
}

QString Interpret_As_UTF_16::decodeUtf16(const QByteArray& fileContent) {
    // Check for BOM (Byte Order Mark) to determine endianness
    if (fileContent.startsWith("\xFF\xFE")) {
        qDebug() << "[DEBUG] Detected BOM: UTF-16LE";
        return QString::fromUtf16(reinterpret_cast<const char16_t*>(fileContent.constData() + 2),
                                  (fileContent.size() - 2) / 2);
    } else if (fileContent.startsWith("\xFE\xFF")) {
        qDebug() << "[DEBUG] Detected BOM: UTF-16BE";
        QByteArray swappedContent = fileContent.mid(2);
        for (int i = 0; i < swappedContent.size(); i += 2) {
            std::swap(swappedContent[i], swappedContent[i + 1]);
        }
        return QString::fromUtf16(reinterpret_cast<const char16_t*>(swappedContent.constData()),
                                  swappedContent.size() / 2);
    }

    // If no BOM is present, default to UTF-16LE
    qDebug() << "[DEBUG] No BOM detected. Assuming UTF-16LE.";
    return QString::fromUtf16(reinterpret_cast<const char16_t*>(fileContent.constData()),
                              fileContent.size() / 2);
}
