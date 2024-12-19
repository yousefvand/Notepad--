#include "interpret_as_utf_32_be.h"
#include <QStringDecoder>
#include <QFile>
#include <QDebug>

InterpreteAsUTF32BE& InterpreteAsUTF32BE::instance() {
    static InterpreteAsUTF32BE instance;
    return instance;
}

InterpreteAsUTF32BE::InterpreteAsUTF32BE() = default;
InterpreteAsUTF32BE::~InterpreteAsUTF32BE() = default;

void InterpreteAsUTF32BE::execute(QPlainTextEdit* editor) {
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
