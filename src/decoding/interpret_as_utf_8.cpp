#include "interpret_as_utf_8.h"
#include "../codeeditor.h"
#include <QDebug>

Interpret_As_Utf_8& Interpret_As_Utf_8::instance()
{
    static Interpret_As_Utf_8 instance;
    return instance;
}

void Interpret_As_Utf_8::execute(CodeEditor* editor) {
    if (!editor) {
        qDebug() << "[ERROR] CodeEditor instance is null.";
        return;
    }

    // Get the raw data
    QString rawData = editor->toPlainText();
    QByteArray utf8Data = rawData.toUtf8();

    // Debug raw data
    qDebug() << "[DEBUG] Original Raw Data (Hex):" << utf8Data.toHex();

    // Decode as UTF-8
    QString decodedText = QString::fromUtf8(utf8Data);
    if (decodedText.isEmpty()) {
        qWarning() << "[WARNING] Decoded UTF-8 text is empty.";
    } else {
        editor->setPlainText(decodedText);
        qDebug() << "[DEBUG] Document interpreted as UTF-8.";
    }
}
