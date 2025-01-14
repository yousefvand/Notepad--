#pragma once

#include <QString>
#include <QByteArray>

struct Utf7HandlerState {
    unsigned long accum = 0;
    int bits = 0;
    unsigned flags = 0;
    unsigned high = 0;
};

class Utf7Handler {
public:
    static Utf7Handler& instance();

    QByteArray encodeUtf7(const QString& text);
    QString decodeUtf7(const QByteArray& utf7Data);

private:
    Utf7Handler();
    ~Utf7Handler();
    Utf7Handler(const Utf7Handler&) = delete;
    Utf7Handler& operator=(const Utf7Handler&) = delete;

    void initUtf7HandlerState(Utf7HandlerState& state) const;
    int encodeCharacterToUtf7(Utf7HandlerState& state, long c, QByteArray& result) const;
    long decodeCharacterFromUtf7(Utf7HandlerState& state, const char*& data, size_t& len) const;

    bool isDirectCharacter(char c) const;
    int base64Encode(int value) const;
    int base64Decode(char c) const;
};

