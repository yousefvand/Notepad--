#include "../codeeditor.h"
#include "interpret_as_euc_jp.h"
#include <QFile>
#include <QDebug>
#include <QStringDecoder>

// Singleton instance
Interpret_As_EUC_JP& Interpret_As_EUC_JP::instance() {
    static Interpret_As_EUC_JP instance;
    return instance;
}

// Constructor
Interpret_As_EUC_JP::Interpret_As_EUC_JP() {}

// Main execution function
void Interpret_As_EUC_JP::execute(QPlainTextEdit* editor) {
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

    QString decodedText = decodeEUCJP(rawData);
    editor->setPlainText(decodedText);
    qDebug() << "[DEBUG] EUC-JP Decoding applied for file:" << filePath;
}

// Decode EUC-JP encoded data
QString Interpret_As_EUC_JP::decodeEUCJP(const QByteArray& rawData) {
    QStringDecoder decoder("EUC-JP");

    if (!decoder.isValid()) {
        qWarning() << "[ERROR] EUC-JP decoder is invalid.";
        return QString();
    }

    return decoder.decode(rawData);
}
