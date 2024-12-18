#include "utf7handler.h"
#include <QDebug>

// Singleton instance
Utf7Handler& Utf7Handler::instance() {
    static Utf7Handler instance;
    return instance;
}

// Constructor and Destructor
Utf7Handler::Utf7Handler() = default;
Utf7Handler::~Utf7Handler() = default;

// Initialize Utf7Handler state
void Utf7Handler::initUtf7HandlerState(Utf7HandlerState& state) const {
    state.accum = 0;
    state.bits = 0;
    state.flags = 0;
    state.high = 0;
}

// Base64 Encoding
int Utf7Handler::base64Encode(int value) const {
    static const char base64Table[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    return base64Table[value & 0x3F];
}

// Base64 Decoding
int Utf7Handler::base64Decode(char c) const {
    static const signed char inv[128] = {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
        -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1
    };

    unsigned char uc = static_cast<unsigned char>(c); // Ensure value is unsigned
    return (uc < 128) ? inv[uc] : -1;
}

// Check if a character is direct
bool Utf7Handler::isDirectCharacter(char c) const {
    return c >= 32 && c <= 126 && c != '+'; // Simplified direct character check
}

// Encode to UTF-7
QByteArray Utf7Handler::encodeUtf7(const QString& text) {
    Utf7HandlerState state;
    initUtf7HandlerState(state);

    QByteArray result;
    for (QChar ch : text) {
        long codePoint = ch.unicode();
        encodeCharacterToUtf7(state, codePoint, result);
    }
    encodeCharacterToUtf7(state, -1, result); // Flush remaining bits
    return result;
}

// Decode from UTF-7
QString Utf7Handler::decodeUtf7(const QByteArray& utf7Data) {
    Utf7HandlerState state;
    initUtf7HandlerState(state);

    QString result;
    const char* data = utf7Data.constData();
    size_t len = utf7Data.size();

    while (len > 0) {
        long decodedChar = decodeCharacterFromUtf7(state, data, len);
        if (decodedChar < 0) break;
        if (decodedChar >= 0 && decodedChar <= 0xFFFF) {
            result += QChar(static_cast<ushort>(decodedChar));
        } else {
            qWarning() << "[WARNING] Invalid UTF-7 decoded character:" << decodedChar;
        }
    }
    return result;
}

// Encode a single character to UTF-7
int Utf7Handler::encodeCharacterToUtf7(Utf7HandlerState& state, long c, QByteArray& result) const {
    if (c == -1) { // Flush remaining bits
        if (state.bits > 0) {
            result.append(base64Encode(state.accum << (6 - state.bits)));
        }
        return 0;
    }

    if (isDirectCharacter(c)) {
        // Convert QChar to its UTF-8 representation and append it
        QString charAsString = QChar(static_cast<ushort>(c));
        result.append(charAsString.toUtf8());
        return 0;
    }

    state.accum = (state.accum << 16) | c;
    state.bits += 16;

    while (state.bits >= 6) {
        result.append(base64Encode(state.accum >> (state.bits - 6)));
        state.bits -= 6;
    }
    return 0;
}

// Decode a single character from UTF-7
long Utf7Handler::decodeCharacterFromUtf7(Utf7HandlerState& state, const char*& data, size_t& len) const {
    int value = base64Decode(*data++);
    len--;

    if (value < 0) return -1;

    state.accum = (state.accum << 6) | value;
    state.bits += 6;

    if (state.bits >= 16) {
        state.bits -= 16;
        return (state.accum >> state.bits) & 0xFFFF;
    }
    return -1; // Incomplete character
}
