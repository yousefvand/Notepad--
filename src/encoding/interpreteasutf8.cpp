#include "interpreteasutf8.h"
#include "../codeeditor.h"
#include <QDebug>

InterpreteAsUtf8& InterpreteAsUtf8::instance()
{
    static InterpreteAsUtf8 instance;
    return instance;
}

void InterpreteAsUtf8::execute(CodeEditor* editor)
{
    if (!editor) {
        qDebug() << "Error: No CodeEditor instance provided.";
        return;
    }

    // Retrieve the current document content as raw bytes
    QByteArray rawData = editor->toPlainText().toUtf8();
    qDebug() << "Original Raw Data (Hex):" << rawData.toHex();

    // Interpret the raw data as UTF-8
    QString interpretedText = QString::fromUtf8(rawData);

    if (interpretedText.isEmpty() && !rawData.isEmpty()) {
        qDebug() << "Error: Failed to interpret document as UTF-8.";
        return;
    }

    // Set the interpreted text back into the editor
    editor->setPlainText(interpretedText);
    qDebug() << "Document interpreted as UTF-8.";
}
