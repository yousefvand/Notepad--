#include "convert_to_utf_8.h"
#include "../codeeditor.h"
#include <QDebug>

Convert_To_Utf_8 & Convert_To_Utf_8::instance()
{
    static Convert_To_Utf_8 instance;
    return instance;
}

void Convert_To_Utf_8::execute(CodeEditor* editor) {
    if (!editor) {
        qDebug() << "[ERROR] CodeEditor instance is null.";
        return;
    }

    QString text = editor->toPlainText();
    QByteArray utf8Data = text.toUtf8();
    QString utf8Text = QString::fromUtf8(utf8Data);
    editor->setPlainText(utf8Text);
}
