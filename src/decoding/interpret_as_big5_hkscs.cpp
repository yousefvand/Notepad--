#include "../codeeditor.h"
#include "interpret_as_big5_hkscs.h"
#include <QFile>
#include <QDebug>
#include <QStringDecoder>

// Singleton instance
Interpret_As_Big5_HKSCS& Interpret_As_Big5_HKSCS::instance() {
    static Interpret_As_Big5_HKSCS instance;
    return instance;
}

// Constructor
Interpret_As_Big5_HKSCS::Interpret_As_Big5_HKSCS() {}

// Main execution function
void Interpret_As_Big5_HKSCS::execute(QPlainTextEdit* editor) {
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

    QString decodedText = decodeBig5HKSCS(rawData);
    editor->setPlainText(decodedText);
    qDebug() << "[DEBUG] Big5-HKSCS Decoding applied for file:" << filePath;
}

// Decode Big5-HKSCS encoded data
QString Interpret_As_Big5_HKSCS::decodeBig5HKSCS(const QByteArray& rawData) {
    QStringDecoder decoder("Big5-HKSCS");

    if (!decoder.isValid()) {
        qWarning() << "[ERROR] Big5-HKSCS decoder is invalid.";
        return QString();
    }

    return decoder.decode(rawData);
}
