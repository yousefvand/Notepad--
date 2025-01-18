#include "../codeeditor.h"
#include "interpret_as_big5.h"
#include <QFile>
#include <QDebug>
#include <QStringDecoder>

// Singleton instance
Interpret_As_Big5& Interpret_As_Big5::instance() {
    static Interpret_As_Big5 instance;
    return instance;
}

// Constructor
Interpret_As_Big5::Interpret_As_Big5() {}

// Main execution function
void Interpret_As_Big5::execute(QPlainTextEdit* editor) {
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

    QString decodedText = decodeBig5(rawData);
    editor->setPlainText(decodedText);
    qDebug() << "[DEBUG] Big5 Decoding applied for file:" << filePath;
}

// Decode Big5 encoded data
QString Interpret_As_Big5::decodeBig5(const QByteArray& rawData) {
    QStringDecoder decoder("Big5");

    if (!decoder.isValid()) {
        qWarning() << "[ERROR] Big5 decoder is invalid.";
        return QString();
    }

    return decoder.decode(rawData);
}
