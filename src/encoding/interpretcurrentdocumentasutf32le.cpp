#include "interpretcurrentdocumentasutf32le.h"
#include <QDebug>
#include <QPlainTextEdit>
#include <QFile>
#include <QStringDecoder>
#include <QTextStream>
#include <QFont>
#include <QStringConverter>

InterpreteAsUTF32LE& InterpreteAsUTF32LE::instance()
{
    static InterpreteAsUTF32LE instance;
    return instance;
}

InterpreteAsUTF32LE::InterpreteAsUTF32LE() = default;

InterpreteAsUTF32LE::~InterpreteAsUTF32LE() = default;

QString InterpreteAsUTF32LE::fromUTF32LE(const QByteArray& data)
{
    if (data.size() % 4 != 0) {
        qWarning() << "[WARNING] Invalid UTF-32LE data size:" << data.size();
        return QString();
    }

    QString result;
    const char32_t* utf32Data = reinterpret_cast<const char32_t*>(data.constData());
    int length = data.size() / 4;

    for (int i = 0; i < length; ++i) {
        char32_t codePoint = utf32Data[i];
        result.append(QChar::fromUcs4(codePoint));
    }

    return result;
}

void InterpreteAsUTF32LE::execute(QPlainTextEdit* editor)
{
    if (!editor) {
        qDebug() << "[ERROR] No QPlainTextEdit instance provided.";
        return; // Exit early if the editor is null
    }

    // Step 1: Retrieve the original text from the editor
    QString originalText = editor->toPlainText();
    qDebug() << "[DEBUG] Original text from editor:" << originalText;

    // Step 2: Convert the text to UTF-32LE encoded data
    QByteArray utf32leData = originalText.toUtf8(); // Assume the editor holds UTF-8 by default

    // Step 3: Decode the UTF-32LE data using QStringDecoder
    QStringDecoder decoder(QStringDecoder::Utf32LE);
    QString decodedText = decoder.decode(utf32leData);

    qDebug() << "[DEBUG] Decoded text as UTF-32LE:" << decodedText;

    // Step 4: Set the decoded text back into the editor
    if (!decodedText.isEmpty()) {
        editor->setPlainText(decodedText);
        qDebug() << "[DEBUG] Updated editor content with decoded UTF-32LE text.";
    } else {
        qWarning() << "[WARNING] Decoding failed or resulted in empty content.";
    }
}
