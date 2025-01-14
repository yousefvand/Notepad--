#include "interpret_as_utf_8_without_bom.h"
#include "../codeeditor.h"
#include <QDebug>

Interpret_As_UTF_8_Without_BOM& Interpret_As_UTF_8_Without_BOM::instance()
{
    static Interpret_As_UTF_8_Without_BOM instance;
    return instance;
}

void Interpret_As_UTF_8_Without_BOM::execute(CodeEditor* editor)
{
    if (!editor) {
        qDebug() << "Error: No CodeEditor instance provided.";
        return;
    }

    // Get the current text as raw bytes
    QByteArray rawData = editor->toPlainText().toUtf8();

    // Remove BOM if present
    if (rawData.startsWith("\xEF\xBB\xBF")) {
        rawData.remove(0, 3); // Remove the first three bytes of BOM
    }

    // Convert back to QString
    QString utf8Text = QString::fromUtf8(rawData);

    // Update the editor content with the UTF-8 without BOM
    editor->setPlainText(utf8Text);

    qDebug() << "Document successfully reinterpreted as UTF-8 without BOM.";
}
