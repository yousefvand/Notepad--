#include <QStringDecoder>
#include <QFile>
#include <QDebug>
#include "interpret_as_tis_620.h"
#include "../codeeditor.h"

// Singleton instance definition
Interpret_As_TIS_620& Interpret_As_TIS_620::instance() {
    static Interpret_As_TIS_620 instance;
    return instance;
}

void Interpret_As_TIS_620::execute(QPlainTextEdit* editor) {
    if (!editor) {
        qWarning() << "[ERROR] No editor instance provided.";
        return;
    }

    CodeEditor* codeEditor = qobject_cast<CodeEditor*>(editor);
    if (!codeEditor) {
        qWarning() << "[ERROR] Editor is not a CodeEditor instance.";
        return;
    }

    QString filePath = codeEditor->filePath();
    if (filePath.isEmpty()) {
        qWarning() << "[ERROR] No file path associated with the editor.";
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "[ERROR] Cannot open file: " << filePath;
        return;
    }

    QByteArray tis620Data = file.readAll();
    file.close();

    // Use QStringDecoder to decode TIS-620
    QStringDecoder decoder("TIS-620");

    // Decode TIS-620 data to QString
    QString decodedText = decoder.decode(tis620Data);

    // Replace the text in the editor with the correctly decoded text
    codeEditor->setPlainText(decodedText);

    qDebug() << "[DEBUG] Successfully reloaded and decoded as TIS-620:" << filePath;
}
