#include "../codeeditor.h"
#include "interpret_as_ibm_866.h"
#include <QFile>
#include <QDebug>

// Singleton instance
Interpret_As_IBM_866& Interpret_As_IBM_866::instance() {
    static Interpret_As_IBM_866 instance;
    return instance;
}

// EBCDIC to Unicode mapping for IBM-866 (Cyrillic EBCDIC)
const std::unordered_map<unsigned char, QChar> Interpret_As_IBM_866::ebcdicTable = {
    {0x80, QChar(0x0410)},  // А
    {0x81, QChar(0x0411)},  // Б
    {0x82, QChar(0x0412)},  // В
    {0x83, QChar(0x0413)},  // Г
    {0x84, QChar(0x0414)},  // Д
    {0x85, QChar(0x0415)},  // Е
    {0x86, QChar(0x0416)},  // Ж
    {0x87, QChar(0x0417)},  // З
    {0x88, QChar(0x0418)},  // И
    {0x89, QChar(0x0419)},  // Й
    {0x8A, QChar(0x041A)},  // К
    {0x8B, QChar(0x041B)},  // Л
    {0x8C, QChar(0x041C)},  // М
    {0x8D, QChar(0x041D)},  // Н
    {0x8E, QChar(0x041E)},  // О
    {0x8F, QChar(0x041F)},  // П
    {0x90, QChar(0x0420)},  // Р
    {0x91, QChar(0x0421)},  // С
    {0x92, QChar(0x0422)},  // Т
    {0x93, QChar(0x0423)},  // У
    {0x94, QChar(0x0424)},  // Ф
    {0x95, QChar(0x0425)},  // Х
    {0x96, QChar(0x0426)},  // Ц
    {0x97, QChar(0x0427)},  // Ч
    {0x98, QChar(0x0428)},  // Ш
    {0x99, QChar(0x0429)},  // Щ
    {0x9A, QChar(0x042A)},  // Ъ
    {0x9B, QChar(0x042B)},  // Ы
    {0x9C, QChar(0x042C)},  // Ь
    {0x9D, QChar(0x042D)},  // Э
    {0x9E, QChar(0x042E)},  // Ю
    {0x9F, QChar(0x042F)},  // Я
    {0xA0, QChar(0x0430)},  // а
    {0xA1, QChar(0x0431)},  // б
    {0xA2, QChar(0x0432)},  // в
    {0xA3, QChar(0x0433)},  // г
    {0xA4, QChar(0x0434)},  // д
    {0xA5, QChar(0x0435)},  // е
    {0xA6, QChar(0x0436)},  // ж
    {0xA7, QChar(0x0437)},  // з
    {0xA8, QChar(0x0438)},  // и
    {0xA9, QChar(0x0439)},  // й
    {0xAA, QChar(0x043A)},  // к
    {0xAB, QChar(0x043B)},  // л
    {0xAC, QChar(0x043C)},  // м
    {0xAD, QChar(0x043D)},  // н
    {0xAE, QChar(0x043E)},  // о
    {0xAF, QChar(0x043F)},  // п
};

// Constructor
Interpret_As_IBM_866::Interpret_As_IBM_866() {}

// Main execution function
void Interpret_As_IBM_866::execute(QPlainTextEdit* editor) {
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

    QString decodedText = decodeIBM866(rawData);
    editor->setPlainText(decodedText);
    qDebug() << "[DEBUG] IBM-866 Decoding applied for file:" << filePath;
}

// Decode raw data from IBM-866 encoding
QString Interpret_As_IBM_866::decodeIBM866(const QByteArray& rawData) {
    QString result;

    for (unsigned char byte : rawData) {
        if (ebcdicTable.contains(byte)) {
            result.append(ebcdicTable.at(byte));
        } else {
            result.append(QChar(0xFFFD));  // Fallback for unmapped characters
        }
    }
    return result;
}
