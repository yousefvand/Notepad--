#include "../codeeditor.h"
#include "interpret_as_hp_roman8.h"
#include <QFile>
#include <QDebug>

// Singleton instance
Interpret_As_hp_roman8& Interpret_As_hp_roman8::instance() {
    static Interpret_As_hp_roman8 instance;
    return instance;
}

// Constructor
Interpret_As_hp_roman8::Interpret_As_hp_roman8() {}

// Main execution function
void Interpret_As_hp_roman8::execute(QPlainTextEdit* editor) {
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

    QString decodedText = decodeHPRoman8(rawData);
    editor->setPlainText(decodedText);
    qDebug() << "[DEBUG] HP Roman8 decoding applied for file:" << filePath;
}

// Decode HP Roman8 encoded data
QString Interpret_As_hp_roman8::decodeHPRoman8(const QByteArray& rawData) {
    QString result;

    for (unsigned char byte : rawData) {
        auto it = hpRoman8Table.find(byte);
        if (it != hpRoman8Table.end()) {
            result.append(it->second);
        } else {
            result.append(QChar(0xFFFD)); // Replacement character for unknown bytes
        }
    }

    return result;
}

// HP Roman8 encoding table
const std::unordered_map<unsigned char, QChar> Interpret_As_hp_roman8::hpRoman8Table = {
    {0x20, QChar(' ')}, {0x21, QChar('!')}, {0x22, QChar('"')}, {0x23, QChar('#')},
    {0x24, QChar('$')}, {0x25, QChar('%')}, {0x26, QChar('&')}, {0x27, QChar('\'')},
    {0x28, QChar('(')}, {0x29, QChar(')')}, {0x2A, QChar('*')}, {0x2B, QChar('+')},
    {0x2C, QChar(',')}, {0x2D, QChar('-')}, {0x2E, QChar('.')}, {0x2F, QChar('/')},
    {0x30, QChar('0')}, {0x31, QChar('1')}, {0x32, QChar('2')}, {0x33, QChar('3')},
    {0x34, QChar('4')}, {0x35, QChar('5')}, {0x36, QChar('6')}, {0x37, QChar('7')},
    {0x38, QChar('8')}, {0x39, QChar('9')}, {0x3A, QChar(':')}, {0x3B, QChar(';')},
    {0x3C, QChar('<')}, {0x3D, QChar('=')}, {0x3E, QChar('>')}, {0x3F, QChar('?')},
    {0x40, QChar('@')}, {0x41, QChar('A')}, {0x42, QChar('B')}, {0x43, QChar('C')},
    {0x44, QChar('D')}, {0x45, QChar('E')}, {0x46, QChar('F')}, {0x47, QChar('G')},
    {0x48, QChar('H')}, {0x49, QChar('I')}, {0x4A, QChar('J')}, {0x4B, QChar('K')},
    {0x4C, QChar('L')}, {0x4D, QChar('M')}, {0x4E, QChar('N')}, {0x4F, QChar('O')},
    {0x50, QChar('P')}, {0x51, QChar('Q')}, {0x52, QChar('R')}, {0x53, QChar('S')},
    {0x54, QChar('T')}, {0x55, QChar('U')}, {0x56, QChar('V')}, {0x57, QChar('W')},
    {0x58, QChar('X')}, {0x59, QChar('Y')}, {0x5A, QChar('Z')}, {0x5B, QChar('[')},
    {0x5C, QChar('\\')}, {0x5D, QChar(']')}, {0x5E, QChar('^')}, {0x5F, QChar('_')},
    {0x60, QChar('`')}, {0x61, QChar('a')}, {0x62, QChar('b')}, {0x63, QChar('c')},
    {0x64, QChar('d')}, {0x65, QChar('e')}, {0x66, QChar('f')}, {0x67, QChar('g')},
    {0x68, QChar('h')}, {0x69, QChar('i')}, {0x6A, QChar('j')}, {0x6B, QChar('k')},
    {0x6C, QChar('l')}, {0x6D, QChar('m')}, {0x6E, QChar('n')}, {0x6F, QChar('o')},
    {0x70, QChar('p')}, {0x71, QChar('q')}, {0x72, QChar('r')}, {0x73, QChar('s')},
    {0x74, QChar('t')}, {0x75, QChar('u')}, {0x76, QChar('v')}, {0x77, QChar('w')},
    {0x78, QChar('x')}, {0x79, QChar('y')}, {0x7A, QChar('z')}, {0x7B, QChar('{')},
    {0x7C, QChar('|')}, {0x7D, QChar('}')}, {0x7E, QChar('~')},
    // Add extended mappings (0x80–0xFF) here...
};
