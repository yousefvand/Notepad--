#include "interpret_as_utf_32_le.h"
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
        qDebug() << "[ERROR] QPlainTextEdit instance is null.";
        return;
    }

    QString filePath = editor->property("currentFilePath").toString();
    if (filePath.isEmpty()) {
        qDebug() << "[ERROR] No file path associated with the editor.";
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "[ERROR] Unable to open file:" << filePath;
        return;
    }

    QByteArray fileContent = file.readAll();
    file.close();

    qDebug() << "[DEBUG] Raw File Content (Hex):" << fileContent.toHex();

    if (fileContent.startsWith("\xFF\xFE\x00\x00")) {
        qDebug() << "[DEBUG] BOM detected: UTF-32LE";
        fileContent.remove(0, 4);
    }

    if (fileContent.size() % 4 != 0) {
        qDebug() << "[ERROR] File size is not a multiple of 4. Invalid UTF-32LE encoding.";
        return;
    }

    QString decodedText = QString::fromUtf16(reinterpret_cast<const char16_t*>(fileContent.constData()),
                                             fileContent.size() / 2);

    if (decodedText.isEmpty()) {
        qDebug() << "[WARNING] Decoded text is empty. Possible decoding issue.";
        return;
    }

    qDebug() << "[DEBUG] Decoded Text:" << decodedText;

    editor->setPlainText(decodedText);
    editor->setFont(QFont("Noto Sans", 12)); // Ensure font supports Unicode
    qDebug() << "[DEBUG] Document interpreted as UTF-32LE.";
}
