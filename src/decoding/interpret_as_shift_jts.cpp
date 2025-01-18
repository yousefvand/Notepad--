#include "interpret_as_shift_jts.h"
#include <QStringDecoder>
#include <QFile>
#include <QDebug>

// Define the constructor
Interpret_As_Shift_JTS::Interpret_As_Shift_JTS() = default;

// Define the singleton instance
Interpret_As_Shift_JTS& Interpret_As_Shift_JTS::instance() {
    static Interpret_As_Shift_JTS instance;
    return instance;
}

QString Interpret_As_Shift_JTS::decodeShiftJTS(const QByteArray& shiftJTSData) {
    QStringDecoder decoder("Shift-JIS");
    if (!decoder.isValid()) {
        qWarning() << "QStringDecoder failed to initialize for Shift-JIS.";
        return QString();
    }
    return decoder.decode(shiftJTSData);
}

void Interpret_As_Shift_JTS::execute(QPlainTextEdit* editor) {
    if (!editor) return;

    QString filePath = editor->property("filePath").toString();
    if (filePath.isEmpty()) {
        qWarning() << "No file path associated with the editor.";
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file for reading:" << filePath;
        return;
    }

    QByteArray fileData = file.readAll();
    file.close();

    qDebug() << "Raw Shift-JIS Data (Hex):" << fileData.toHex();

    QString decodedText = decodeShiftJTS(fileData);
    if (decodedText.isEmpty()) {
        qWarning() << "Failed to decode Shift-JIS text.";
        return;
    }
    qDebug() << "Decoded Text:" << decodedText;

    editor->setPlainText(decodedText);
}
