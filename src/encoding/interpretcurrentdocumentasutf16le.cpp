#include "interpretcurrentdocumentasutf16le.h"
#include "../codeeditor.h"
#include <QDebug>

InterpretCurrentDocumentAsUTF16LE& InterpretCurrentDocumentAsUTF16LE::instance()
{
    static InterpretCurrentDocumentAsUTF16LE instance;
    return instance;
}

InterpretCurrentDocumentAsUTF16LE::InterpretCurrentDocumentAsUTF16LE() = default;

InterpretCurrentDocumentAsUTF16LE::~InterpretCurrentDocumentAsUTF16LE() = default;

void InterpretCurrentDocumentAsUTF16LE::execute(CodeEditor* editor)
{
    if (!editor) {
        qDebug() << "Error: No CodeEditor instance provided.";
        return;
    }

    // Get the raw content of the document
    QByteArray rawData = editor->toPlainText().toUtf8(); // Ensure raw data
    qDebug() << "Original Raw Data (Hex):" << rawData.toHex();

    // Reinterpret as UTF-16LE
    QString interpretedText = QString::fromUtf16(
        reinterpret_cast<const char16_t*>(rawData.constData()), rawData.size() / 2);

    qDebug() << "Interpreted UTF-16LE Text:" << interpretedText;

    // Update the editor content
    editor->setPlainText(interpretedText);

    qDebug() << "Document reinterpreted as UTF-16LE.";
}
