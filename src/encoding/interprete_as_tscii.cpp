#include "interprete_as_tscii.h"
#include <QDebug>
#include <QByteArray>
#include <QMap>

// Constructor
Interprete_As_TSCII::Interprete_As_TSCII() = default;

// Destructor definition
Interprete_As_TSCII::~Interprete_As_TSCII() = default;

// Singleton instance
Interprete_As_TSCII& Interprete_As_TSCII::instance() {
    static Interprete_As_TSCII instance;
    return instance;
}

// Helper: TSCII to Unicode mapping
static QMap<unsigned char, QChar> createTSCIIMap() {
    QMap<unsigned char, QChar> map;
    map[0xA1] = QChar(0x0B85); // Example: TSCII 0xA1 -> Unicode 'அ'
    map[0xA2] = QChar(0x0B86); // Example: TSCII 0xA2 -> Unicode 'ஆ'
    // Add remaining mappings as needed
    return map;
}

static const QMap<unsigned char, QChar>& getTSCIIMap() {
    static QMap<unsigned char, QChar> tsciiMap = createTSCIIMap();
    return tsciiMap;
}

// Helper: Decode TSCII to QString
QString decodeTSCII(const QByteArray& data) {
    QString result;
    const auto& map = getTSCIIMap();
    for (unsigned char c : data) {
        if (map.contains(c)) {
            result += map.value(c);
        } else {
            result += QChar(0xFFFD); // Unicode replacement character
        }
    }
    return result;
}

// Execute TSCII interpretation
void Interprete_As_TSCII::execute(QPlainTextEdit* editor) {
    if (!editor) {
        qDebug() << "[ERROR] No editor provided.";
        return;
    }

    QString originalText = editor->toPlainText();
    QByteArray tsciiData = originalText.toLocal8Bit();
    QString decodedText = decodeTSCII(tsciiData);

    qDebug() << "[DEBUG] Decoded TSCII text:" << decodedText;
    editor->setPlainText(decodedText);
}
