#include "interpret_as_utf_16_be.h"
#include "../codeeditor.h"
#include <QDebug>

Interpret_As_UTF_16_BE& Interpret_As_UTF_16_BE::instance()
{
    static Interpret_As_UTF_16_BE instance;
    return instance;
}

void Interpret_As_UTF_16_BE::execute(CodeEditor* editor)
{
    if (!editor) {
        qDebug() << "Error: No CodeEditor instance provided.";
        return;
    }

    // Get the current text from the editor
    QString text = editor->toPlainText();

    // Log the original text
    qDebug() << "Original text:" << text;

    // Convert the text to UTF-16BE
    QByteArray utf16beData;
    const char16_t* utf16Data = reinterpret_cast<const char16_t*>(text.utf16());
    qsizetype utf16Length = text.size();

    for (qsizetype i = 0; i < utf16Length; ++i) {
        char16_t codeUnit = utf16Data[i];

        // Convert to big-endian format
        utf16beData.append(static_cast<char>((codeUnit >> 8) & 0xFF));  // High byte
        utf16beData.append(static_cast<char>(codeUnit & 0xFF));         // Low byte
    }

    // Log the UTF-16BE data
    qDebug() << "UTF-16BE data:" << utf16beData.toHex();

    // Correctly interpret UTF-16BE as QString
    QByteArray tempData;
    for (qsizetype i = 0; i < utf16beData.size(); i += 2) {
        // Convert back to native-endian format for QString::fromUtf16
        tempData.append(utf16beData[i + 1]); // Low byte
        tempData.append(utf16beData[i]);     // High byte
    }

    // Interpret the text using native-endian conversion
    QString interpretedText = QString::fromUtf16(
        reinterpret_cast<const char16_t*>(tempData.constData()),
        tempData.size() / 2);

    // Log the interpreted text
    qDebug() << "Interpreted text:" << interpretedText;

    // Update the editor content
    editor->setPlainText(interpretedText);

    qDebug() << "Document successfully reinterpreted as UTF-16BE.";
}
