#include "../codeeditor.h"
#include "interpret_as_ibm_420.h"
#include <QFile>
#include <QDebug>

// Singleton instance
Interpret_As_IBM_420& Interpret_As_IBM_420::instance() {
    static Interpret_As_IBM_420 instance;
    return instance;
}

// EBCDIC to Unicode mapping for IBM-420 (Arabic EBCDIC)
const std::unordered_map<unsigned char, QChar> Interpret_As_IBM_420::ebcdicTable = {
    {0x80, QChar(0x0627)},  // ا (Alef)
    {0x81, QChar(0x0628)},  // ب (Beh)
    {0x82, QChar(0x062A)},  // ت (Teh)
    {0x83, QChar(0x062B)},  // ث (Theh)
    {0x84, QChar(0x062C)},  // ج (Jeem)
    {0x85, QChar(0x062D)},  // ح (Hah)
    {0x86, QChar(0x062E)},  // خ (Khah)
    {0x87, QChar(0x062F)},  // د (Dal)
    {0x88, QChar(0x0630)},  // ذ (Thal)
    {0x89, QChar(0x0631)},  // ر (Reh)
    {0x8A, QChar(0x0632)},  // ز (Zain)
    {0x8B, QChar(0x0633)},  // س (Seen)
    {0x8C, QChar(0x0634)},  // ش (Sheen)
    {0x8D, QChar(0x0635)},  // ص (Sad)
    {0x8E, QChar(0x0636)},  // ض (Dad)
    {0x8F, QChar(0x0637)},  // ط (Tah)
    {0x90, QChar(0x0638)},  // ظ (Zah)
    {0x91, QChar(0x0639)},  // ع (Ain)
    {0x92, QChar(0x063A)},  // غ (Ghain)
    {0x93, QChar(0x0641)},  // ف (Feh)
    {0x94, QChar(0x0642)},  // ق (Qaf)
    {0x95, QChar(0x0643)},  // ك (Kaf)
    {0x96, QChar(0x0644)},  // ل (Lam)
    {0x97, QChar(0x0645)},  // م (Meem)
    {0x98, QChar(0x0646)},  // ن (Noon)
    {0x99, QChar(0x0647)},  // ه (Heh)
    {0x9A, QChar(0x0648)},  // و (Waw)
    {0x9B, QChar(0x064A)},  // ي (Yeh)
};

// Constructor
Interpret_As_IBM_420::Interpret_As_IBM_420() {}

// Main execution function
void Interpret_As_IBM_420::execute(QPlainTextEdit* editor) {
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

    QString decodedText = decodeIBM420(rawData);
    editor->setPlainText(decodedText);
    qDebug() << "[DEBUG] IBM-420 Decoding applied for file:" << filePath;
}

// Decode raw data from IBM-420 encoding
QString Interpret_As_IBM_420::decodeIBM420(const QByteArray& rawData) {
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
