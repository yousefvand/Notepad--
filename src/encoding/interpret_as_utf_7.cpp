#include "interpret_as_utf_7.h"
#include <QDebug>

// Singleton instance
InterpreteAsUtf7& InterpreteAsUtf7::instance()
{
    static InterpreteAsUtf7 instance;
    return instance;
}

InterpreteAsUtf7::InterpreteAsUtf7() = default;
InterpreteAsUtf7::~InterpreteAsUtf7() = default;

// Main UTF-7 decoding function
QString InterpreteAsUtf7::fromUtf7(const QByteArray& utf7Data)
{
    QString result;
    QByteArray base64Buffer;
    bool inBase64 = false;  // Tracks if we are inside a Base64-encoded section
    bool decodingError = false;

    qDebug() << "[DEBUG] Starting UTF-7 decoding.";

    for (char c : utf7Data) {
        if (c == '+') {
            // Handle the start of a Base64 section
            if (inBase64) {
                // If we encounter another '+' without closing '-', treat previous as literal
                qWarning() << "[WARNING] Unclosed Base64 section detected. Treating as literal.";
                result += "+" + base64Buffer;
                base64Buffer.clear();
            }
            inBase64 = true; // Enter Base64 mode
            continue;
        }

        if (c == '-' && inBase64) {
            // Handle the end of a Base64 section
            QString decodedSegment = processBase64Segment(base64Buffer, decodingError);
            if (decodingError) {
                // Fallback: Append as literal Base64 if decoding failed
                qWarning() << "[WARNING] Decoding error; appending literal Base64 data.";
                result += "+" + base64Buffer + "-";
            } else {
                result += decodedSegment; // Append decoded content
            }
            base64Buffer.clear();
            inBase64 = false;
            continue;
        }

        if (inBase64) {
            // Collect Base64 characters
            base64Buffer.append(c);
        } else {
            // Handle literal characters
            result += handleLiteralCharacter(c);
        }
    }

    // Handle unclosed Base64 section at the end
    if (inBase64 && !base64Buffer.isEmpty()) {
        qWarning() << "[WARNING] Unclosed Base64 section at the end. Treating as literal.";
        result += "+" + base64Buffer;
    }

    qDebug() << "[DEBUG] Final UTF-7 decoded result:" << result;
    return result;
}

// Handle literal (non-encoded) characters
QString InterpreteAsUtf7::handleLiteralCharacter(char c)
{
    return QString(c);
}

QByteArray InterpreteAsUtf7::decodeBase64Segment(const QByteArray& base64Segment, bool& errorFlag)
{
    QByteArray decodedBytes = QByteArray::fromBase64(base64Segment);
    if (decodedBytes.isEmpty()) {
        errorFlag = true;
        qWarning() << "[ERROR] Base64 decoding failed for segment:" << base64Segment;
    } else {
        qDebug() << "[DEBUG] Decoded Base64 bytes:" << decodedBytes.toHex();
    }
    return decodedBytes;
}

// Convert decoded bytes to UTF-16 LE
QString InterpreteAsUtf7::convertToUtf16LE(const QByteArray& decodedBytes, bool& errorFlag)
{
    if (decodedBytes.size() % 2 != 0) {
        errorFlag = true;
        qWarning() << "[ERROR] Decoded bytes size is not even for UTF-16 LE conversion.";
        return QString();
    }

    QString result;
    for (int i = 0; i < decodedBytes.size(); i += 2) {
        ushort codeUnit = static_cast<uchar>(decodedBytes[i]) |
                          (static_cast<uchar>(decodedBytes[i + 1]) << 8);
        qDebug() << "[DEBUG] Code unit:" << QString::number(codeUnit, 16);
        result.append(QChar(codeUnit));
    }

    qDebug() << "[DEBUG] UTF-16 LE result:" << result;
    return result;
}

// Check if a character is a valid Base64 character
bool InterpreteAsUtf7::isBase64Character(char c)
{
    return (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z') ||
           (c >= '0' && c <= '9') ||
           c == '+' || c == '/';
}

// Executes UTF-7 decoding on the editor content
void InterpreteAsUtf7::execute(QPlainTextEdit* editor)
{
    if (!editor) {
        qDebug() << "[ERROR] No QPlainTextEdit instance provided.";
        return; // Exit early if the editor is null
    }

    // 1. Retrieve the original text from the editor
    QString originalText = editor->toPlainText();
    qDebug() << "[DEBUG] Original text:" << originalText;

    // 2. Decode the original text from UTF-7
    QByteArray utf7Data = originalText.toUtf8();
    QString decodedText = fromUtf7(utf7Data);

    if (decodedText.isEmpty()) {
        qWarning() << "[WARNING] Decoding failed or resulted in empty content. Keeping original text.";
        return; // Exit without updating the editor if decoding fails
    }

    // 3. Update the editor only if the decoded text differs
    if (decodedText != originalText) {
        editor->setPlainText(decodedText);
        qDebug() << "[DEBUG] Updated editor content with UTF-7 decoded text.";
    } else {
        qDebug() << "[INFO] No changes detected after decoding.";
    }
}

// Decode Base64 segment into UTF-16 Little Endian
QString InterpreteAsUtf7::processBase64Segment(QByteArray& base64Buffer, bool& errorFlag) {
    // Step 1: Decode Base64
    QByteArray decodedBytes = QByteArray::fromBase64(base64Buffer, QByteArray::Base64Encoding);
    if (decodedBytes.isEmpty()) {
        errorFlag = true;
        qWarning() << "[WARNING] Base64 decoding failed for:" << base64Buffer;
        return QString();
    }

    // Step 2: Interpret as UTF-16 Little Endian
    if (decodedBytes.size() % 2 != 0) {
        errorFlag = true;
        qWarning() << "[WARNING] Decoded bytes are not aligned for UTF-16:" << decodedBytes;
        return QString();
    }

    QString decodedText;
    for (int i = 0; i < decodedBytes.size(); i += 2) {
        // Little-endian decoding
        ushort codeUnit = static_cast<uchar>(decodedBytes[i]) |
                          (static_cast<uchar>(decodedBytes[i + 1]) << 8);
        decodedText.append(QChar(codeUnit));
    }

    return decodedText;
}
