#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_Adobe_Standard_Encoding {
public:
    // Singleton instance access
    static Interpret_As_Adobe_Standard_Encoding& instance();

    // Execute decoding for the provided editor
    void execute(QPlainTextEdit* editor);

private:
    // Private constructor for Singleton pattern
    Interpret_As_Adobe_Standard_Encoding();
    ~Interpret_As_Adobe_Standard_Encoding() = default;

    // Disable copy construction and assignment
    Interpret_As_Adobe_Standard_Encoding(const Interpret_As_Adobe_Standard_Encoding&) = delete;
    Interpret_As_Adobe_Standard_Encoding& operator=(const Interpret_As_Adobe_Standard_Encoding&) = delete;

    // Decoding method for Adobe-Standard-Encoding data
    QString decodeAdobeStandardEncoding(const QByteArray& rawData);

    // Adobe Standard Encoding table
    static const std::unordered_map<unsigned char, QChar> adobeStandardEncodingTable;
};
