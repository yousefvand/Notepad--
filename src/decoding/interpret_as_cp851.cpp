#include "../codeeditor.h"
#include "interpret_as_cp851.h"
#include <QFile>
#include <QDebug>

// Singleton instance
Interpret_As_cp851& Interpret_As_cp851::instance() {
    static Interpret_As_cp851 instance;
    return instance;
}

// Constructor
Interpret_As_cp851::Interpret_As_cp851() {}

// Main execution function
void Interpret_As_cp851::execute(QPlainTextEdit* editor) {
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

    QString decodedText = decodeCP851(rawData);
    editor->setPlainText(decodedText);
    qDebug() << "[DEBUG] CP851 decoding applied for file:" << filePath;
}

// Decode CP851 encoded data
QString Interpret_As_cp851::decodeCP851(const QByteArray& rawData) {
    QString result;

    for (unsigned char byte : rawData) {
        auto it = cp851Table.find(byte);
        if (it != cp851Table.end()) {
            result.append(it->second);
        } else {
            result.append(QChar(0xFFFD)); // Replacement character for unknown bytes
        }
    }

    return result;
}

// CP851 encoding table
const std::unordered_map<unsigned char, QChar> Interpret_As_cp851::cp851Table = {
    // ASCII (0x00–0x7F)
    {0x00, QChar(0x0000)}, {0x01, QChar(0x0001)}, {0x02, QChar(0x0002)}, {0x03, QChar(0x0003)},
    {0x04, QChar(0x0004)}, {0x05, QChar(0x0005)}, {0x06, QChar(0x0006)}, {0x07, QChar(0x0007)},
    {0x08, QChar(0x0008)}, {0x09, QChar(0x0009)}, {0x0A, QChar(0x000A)}, {0x0B, QChar(0x000B)},
    {0x0C, QChar(0x000C)}, {0x0D, QChar(0x000D)}, {0x0E, QChar(0x000E)}, {0x0F, QChar(0x000F)},
    {0x10, QChar(0x0010)}, {0x11, QChar(0x0011)}, {0x12, QChar(0x0012)}, {0x13, QChar(0x0013)},
    {0x14, QChar(0x0014)}, {0x15, QChar(0x0015)}, {0x16, QChar(0x0016)}, {0x17, QChar(0x0017)},
    {0x18, QChar(0x0018)}, {0x19, QChar(0x0019)}, {0x1A, QChar(0x001A)}, {0x1B, QChar(0x001B)},
    {0x1C, QChar(0x001C)}, {0x1D, QChar(0x001D)}, {0x1E, QChar(0x001E)}, {0x1F, QChar(0x001F)},
    {0x20, QChar(' ')},    {0x21, QChar('!')},    {0x22, QChar('"')},    {0x23, QChar('#')},
    {0x24, QChar('$')},    {0x25, QChar('%')},    {0x26, QChar('&')},    {0x27, QChar('\'')},
    {0x28, QChar('(')},    {0x29, QChar(')')},    {0x2A, QChar('*')},    {0x2B, QChar('+')},
    {0x2C, QChar(',')},    {0x2D, QChar('-')},    {0x2E, QChar('.')},    {0x2F, QChar('/')},
    {0x30, QChar('0')},    {0x31, QChar('1')},    {0x32, QChar('2')},    {0x33, QChar('3')},
    {0x34, QChar('4')},    {0x35, QChar('5')},    {0x36, QChar('6')},    {0x37, QChar('7')},
    {0x38, QChar('8')},    {0x39, QChar('9')},    {0x3A, QChar(':')},    {0x3B, QChar(';')},
    {0x3C, QChar('<')},    {0x3D, QChar('=')},    {0x3E, QChar('>')},    {0x3F, QChar('?')},
    {0x40, QChar('@')},    {0x41, QChar('A')},    {0x42, QChar('B')},    {0x43, QChar('C')},
    {0x44, QChar('D')},    {0x45, QChar('E')},    {0x46, QChar('F')},    {0x47, QChar('G')},
    {0x48, QChar('H')},    {0x49, QChar('I')},    {0x4A, QChar('J')},    {0x4B, QChar('K')},
    {0x4C, QChar('L')},    {0x4D, QChar('M')},    {0x4E, QChar('N')},    {0x4F, QChar('O')},
    {0x50, QChar('P')},    {0x51, QChar('Q')},    {0x52, QChar('R')},    {0x53, QChar('S')},
    {0x54, QChar('T')},    {0x55, QChar('U')},    {0x56, QChar('V')},    {0x57, QChar('W')},
    {0x58, QChar('X')},    {0x59, QChar('Y')},    {0x5A, QChar('Z')},    {0x5B, QChar('[')},
    {0x5C, QChar('\\')},   {0x5D, QChar(']')},    {0x5E, QChar('^')},    {0x5F, QChar('_')},
    {0x60, QChar('`')},    {0x61, QChar('a')},    {0x62, QChar('b')},    {0x63, QChar('c')},
    {0x64, QChar('d')},    {0x65, QChar('e')},    {0x66, QChar('f')},    {0x67, QChar('g')},
    {0x68, QChar('h')},    {0x69, QChar('i')},    {0x6A, QChar('j')},    {0x6B, QChar('k')},
    {0x6C, QChar('l')},    {0x6D, QChar('m')},    {0x6E, QChar('n')},    {0x6F, QChar('o')},
    {0x70, QChar('p')},    {0x71, QChar('q')},    {0x72, QChar('r')},    {0x73, QChar('s')},
    {0x74, QChar('t')},    {0x75, QChar('u')},    {0x76, QChar('v')},    {0x77, QChar('w')},
    {0x78, QChar('x')},    {0x79, QChar('y')},    {0x7A, QChar('z')},    {0x7B, QChar('{')},
    {0x7C, QChar('|')},    {0x7D, QChar('}')},    {0x7E, QChar('~')},
    // CP851 Extended Characters (0x80–0xFF)
    {0x80, QChar(0x00C7)}, {0x81, QChar(0x00FC)}, {0x82, QChar(0x00E9)}, {0x83, QChar(0x00E2)},
    {0x84, QChar(0x00E4)}, {0x85, QChar(0x00E0)}, {0x86, QChar(0x00E5)}, {0x87, QChar(0x00E7)},
    {0x88, QChar(0x00EA)}, {0x89, QChar(0x00EB)}, {0x8A, QChar(0x00E8)}, {0x8B, QChar(0x00EF)},
    {0x8C, QChar(0x00EE)}, {0x8D, QChar(0x00EC)}, {0x8E, QChar(0x00C4)}, {0x8F, QChar(0x00C5)},
    {0x90, QChar(0x00C9)}, {0x91, QChar(0x00E6)}, {0x92, QChar(0x00C6)}, {0x93, QChar(0x00F4)},
    {0x94, QChar(0x00F6)}, {0x95, QChar(0x00F2)}, {0x96, QChar(0x00FB)}, {0x97, QChar(0x00F9)},
    {0x98, QChar(0x00FF)}, {0x99, QChar(0x00D6)}, {0x9A, QChar(0x00DC)}, {0x9B, QChar(0x00A2)},
    {0x9C, QChar(0x00A3)}, {0x9D, QChar(0x00A5)}, {0x9E, QChar(0x20A7)}, {0x9F, QChar(0x0192)},
    {0xA0, QChar(0x00E1)}, {0xA1, QChar(0x00ED)}, {0xA2, QChar(0x00F3)}, {0xA3, QChar(0x00FA)},
    {0xA4, QChar(0x00F1)}, {0xA5, QChar(0x00D1)}, {0xA6, QChar(0x00AA)}, {0xA7, QChar(0x00BA)},
    {0xA8, QChar(0x00BF)}, {0xA9, QChar(0x2310)}, {0xAA, QChar(0x00AC)}, {0xAB, QChar(0x00BD)},
    {0xAC, QChar(0x00BC)}, {0xAD, QChar(0x00A1)}, {0xAE, QChar(0x00AB)}, {0xAF, QChar(0x00BB)},
    // Remaining mappings...
};
