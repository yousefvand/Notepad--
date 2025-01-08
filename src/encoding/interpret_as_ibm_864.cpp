#include "../codeeditor.h"
#include "interpret_as_ibm_864.h"
#include <QFile>
#include <QDebug>

// Singleton instance
Interpret_As_IBM_864& Interpret_As_IBM_864::instance() {
    static Interpret_As_IBM_864 instance;
    return instance;
}

// EBCDIC to Unicode mapping for IBM-864 (Arabic EBCDIC)
const std::unordered_map<unsigned char, QChar> Interpret_As_IBM_864::ebcdicTable = {
    {0x40, QChar(0x0020)},  // Space
    {0x4A, QChar(0x060C)},  // ، (Arabic Comma)
    {0x5A, QChar(0x061B)},  // ؛ (Arabic Semicolon)
    {0x5F, QChar(0x061F)},  // ؟ (Arabic Question Mark)
    {0x61, QChar(0x0621)},  // ء (Hamza)
    {0x62, QChar(0x0622)},  // آ (Alef with Madda)
    {0x63, QChar(0x0623)},  // أ (Alef with Hamza Above)
    {0x64, QChar(0x0624)},  // ؤ (Waw with Hamza Above)
    {0x65, QChar(0x0625)},  // إ (Alef with Hamza Below)
    {0x66, QChar(0x0626)},  // ئ (Yeh with Hamza Above)
    {0x67, QChar(0x0627)},  // ا (Alef)
    {0x68, QChar(0x0628)},  // ب (Beh)
    {0x69, QChar(0x0629)},  // ة (Teh Marbuta)
    {0x6A, QChar(0x062A)},  // ت (Teh)
    {0x6B, QChar(0x062B)},  // ث (Theh)
    {0x6C, QChar(0x062C)},  // ج (Jeem)
    {0x6D, QChar(0x062D)},  // ح (Hah)
    {0x6E, QChar(0x062E)},  // خ (Khah)
    {0x6F, QChar(0x062F)},  // د (Dal)
    {0x70, QChar(0x0630)},  // ذ (Thal)
    {0x71, QChar(0x0631)},  // ر (Reh)
    {0x72, QChar(0x0632)},  // ز (Zain)
    {0x73, QChar(0x0633)},  // س (Seen)
    {0x74, QChar(0x0634)},  // ش (Sheen)
    {0x75, QChar(0x0635)},  // ص (Sad)
    {0x76, QChar(0x0636)},  // ض (Dad)
    {0x77, QChar(0x0637)},  // ط (Tah)
    {0x78, QChar(0x0638)},  // ظ (Zah)
    {0x79, QChar(0x0639)},  // ع (Ain)
    {0x7A, QChar(0x063A)},  // غ (Ghain)
    {0x7B, QChar(0x0640)},  // ـ (Tatweel)
    {0x7C, QChar(0x0641)},  // ف (Feh)
    {0x7D, QChar(0x0642)},  // ق (Qaf)
    {0x7E, QChar(0x0643)},  // ك (Kaf)
    {0x7F, QChar(0x0644)},  // ل (Lam)
    {0x80, QChar(0x0645)},  // م (Meem)
    {0x81, QChar(0x0646)},  // ن (Noon)
    {0x82, QChar(0x0647)},  // ه (Heh)
    {0x83, QChar(0x0648)},  // و (Waw)
    {0x84, QChar(0x0649)},  // ى (Alef Maksura)
    {0x85, QChar(0x064A)},  // ي (Yeh)
};

// Constructor
Interpret_As_IBM_864::Interpret_As_IBM_864() {}

// Main execution function
void Interpret_As_IBM_864::execute(QPlainTextEdit* editor) {
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

    QString decodedText = decodeIBM864(rawData);
    editor->setPlainText(decodedText);
    qDebug() << "[DEBUG] IBM-864 Decoding applied for file:" << filePath;
}

// Decode raw data from IBM-864 encoding
QString Interpret_As_IBM_864::decodeIBM864(const QByteArray& rawData) {
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
