#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_01144 {
public:
    static Interpret_As_IBM_01144& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_01144();
    ~Interpret_As_IBM_01144() = default;

    Interpret_As_IBM_01144(const Interpret_As_IBM_01144&) = delete;
    Interpret_As_IBM_01144& operator=(const Interpret_As_IBM_01144&) = delete;

    QString decodeIBM01144(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
