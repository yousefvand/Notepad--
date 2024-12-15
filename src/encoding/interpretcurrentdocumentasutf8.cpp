#include "interpretcurrentdocumentasutf8.h"
#include "../codeeditor.h"
#include <QDebug>

// Singleton instance
InterpretCurrentDocumentAsUTF8& InterpretCurrentDocumentAsUTF8::instance()
{
    static InterpretCurrentDocumentAsUTF8 instance;
    return instance;
}

// Execute the reinterpretation of the current document as UTF-8
void InterpretCurrentDocumentAsUTF8::execute(CodeEditor* editor)
{
    if (!editor) {
        qDebug() << "Error: No CodeEditor instance provided.";
        return;
    }

    // Retrieve the raw text from the CodeEditor
    QByteArray rawData = editor->toPlainText().toUtf8();

    // Reinterpret the content as UTF-8
    QString utf8Text = QString::fromUtf8(rawData);

    // Update the editor content with the reinterpreted UTF-8 text
    editor->setPlainText(utf8Text);

    qDebug() << "Document successfully reinterpreted as UTF-8.";
}
