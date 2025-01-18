#include "../codeeditor.h"
#include "interpret_as_gbk.h"
#include <QFile>
#include <QDebug>
#include <QStringDecoder>

// Singleton instance
Interpret_As_GBK& Interpret_As_GBK::instance() {
    static Interpret_As_GBK instance;
    return instance;
}

// Constructor
Interpret_As_GBK::Interpret_As_GBK() {}

// Main execution function
void Interpret_As_GBK::execute(QPlainTextEdit* editor) {
    if (!editor) {
        qWarning() << "[ERROR] No editor instance provided.";
        return;
    }

    CodeEditor* codeEditor = qobject_cast<CodeEditor*>(editor);
    if (!codeEditor) {
        qWarning() << "[ERROR] Invalid CodeEditor instance.";
        return;
    }

    QString filePath = codeEditor->filePath();
    if (filePath.isEmpty()) {
        qWarning() << "[ERROR] No file path associated with the editor.";
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "[ERROR] Cannot open file:" << filePath;
        return;
    }

    QByteArray rawData = file.readAll();
    file.close();

    QString decodedText = decodeGBK(rawData);
    editor->setPlainText(decodedText);
    qDebug() << "[DEBUG] GBK Decoding applied for file:" << filePath;
}

// Decode GBK encoded data
QString Interpret_As_GBK::decodeGBK(const QByteArray& rawData) {
    QStringDecoder decoder("GBK");

    if (!decoder.isValid()) {
        qWarning() << "[ERROR] GBK decoder is invalid.";
        return QString();
    }

    return decoder.decode(rawData);
}
