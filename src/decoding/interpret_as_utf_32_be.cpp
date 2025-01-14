#include "interpret_as_utf_32_be.h"
#include <QStringDecoder>
#include <QFile>
#include <QDebug>

Interpret_As_UTF_32_BE& Interpret_As_UTF_32_BE::instance() {
    static Interpret_As_UTF_32_BE instance;
    return instance;
}

Interpret_As_UTF_32_BE::Interpret_As_UTF_32_BE() = default;
Interpret_As_UTF_32_BE::~Interpret_As_UTF_32_BE() = default;

void Interpret_As_UTF_32_BE::execute(QPlainTextEdit* editor) {
    if (!editor) {
        qWarning() << "[ERROR] No QPlainTextEdit instance provided.";
        return;
    }

    // Attempt to retrieve the file path from the editor's property
    QString filePath = editor->property("filePath").toString();
    if (filePath.isEmpty()) {
        qWarning() << "[ERROR] No file path associated with the editor.";
        return;
    }

    // Open the file for reading
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "[ERROR] Failed to open file for reading:" << filePath;
        return;
    }

    // Read file content
    QByteArray fileData = file.readAll();
    file.close();

    // Decode using UTF-32BE
    QStringDecoder decoder(QStringDecoder::Utf32BE);
    QString decodedText = decoder.decode(fileData);

    if (decodedText.isEmpty()) {
        qWarning() << "[ERROR] Failed to decode file as UTF-32BE or content is empty.";
        return;
    }

    // Update the editor with the decoded content
    editor->setPlainText(decodedText);
    qDebug() << "[INFO] Successfully updated editor content with UTF-32BE decoded text.";
}
