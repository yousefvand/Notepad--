#include "../codeeditor.h"
#include "interpret_as_ibm_869.h"
#include <QFile>
#include <QDebug>

// Singleton instance
Interpret_As_IBM_869& Interpret_As_IBM_869::instance() {
    static Interpret_As_IBM_869 instance;
    return instance;
}

// EBCDIC to Unicode mapping for IBM-869 (Greek EBCDIC)
const std::unordered_map<unsigned char, QChar> Interpret_As_IBM_869::ebcdicTable = {
    {0x40, QChar(0x0020)},  // Space
    {0x4A, QChar(0x0384)},  // ΄ (Greek Tonos)
    {0x5A, QChar(0x003B)},  // ; (Semicolon)
    {0x5F, QChar(0x0387)},  // · (Greek Ano Teleia)
    {0x61, QChar(0x0391)},  // Α (Alpha)
    {0x62, QChar(0x0392)},  // Β (Beta)
    {0x63, QChar(0x0393)},  // Γ (Gamma)
    {0x64, QChar(0x0394)},  // Δ (Delta)
    {0x65, QChar(0x0395)},  // Ε (Epsilon)
    {0x66, QChar(0x0396)},  // Ζ (Zeta)
    {0x67, QChar(0x0397)},  // Η (Eta)
    {0x68, QChar(0x0398)},  // Θ (Theta)
    {0x69, QChar(0x0399)},  // Ι (Iota)
    {0x6A, QChar(0x039A)},  // Κ (Kappa)
    {0x6B, QChar(0x039B)},  // Λ (Lambda)
    {0x6C, QChar(0x039C)},  // Μ (Mu)
    {0x6D, QChar(0x039D)},  // Ν (Nu)
    {0x6E, QChar(0x039E)},  // Ξ (Xi)
    {0x6F, QChar(0x039F)},  // Ο (Omicron)
    {0x70, QChar(0x03A0)},  // Π (Pi)
    {0x71, QChar(0x03A1)},  // Ρ (Rho)
    {0x72, QChar(0x03A3)},  // Σ (Sigma)
    {0x73, QChar(0x03A4)},  // Τ (Tau)
    {0x74, QChar(0x03A5)},  // Υ (Upsilon)
    {0x75, QChar(0x03A6)},  // Φ (Phi)
    {0x76, QChar(0x03A7)},  // Χ (Chi)
    {0x77, QChar(0x03A8)},  // Ψ (Psi)
    {0x78, QChar(0x03A9)},  // Ω (Omega)
    {0x79, QChar(0x03B1)},  // α (alpha)
    {0x7A, QChar(0x03B2)},  // β (beta)
    {0x7B, QChar(0x03B3)},  // γ (gamma)
    {0x7C, QChar(0x03B4)},  // δ (delta)
    {0x7D, QChar(0x03B5)},  // ε (epsilon)
    {0x7E, QChar(0x03B6)},  // ζ (zeta)
    {0x7F, QChar(0x03B7)},  // η (eta)
};

// Constructor
Interpret_As_IBM_869::Interpret_As_IBM_869() {}

// Main execution function
void Interpret_As_IBM_869::execute(QPlainTextEdit* editor) {
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

    QString decodedText = decodeIBM869(rawData);
    editor->setPlainText(decodedText);
    qDebug() << "[DEBUG] IBM-869 Decoding applied for file:" << filePath;
}

// Decode raw data from IBM-869 encoding
QString Interpret_As_IBM_869::decodeIBM869(const QByteArray& rawData) {
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
