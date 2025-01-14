#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_01145 {
public:
    static Interpret_As_IBM_01145& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_01145();
    ~Interpret_As_IBM_01145() = default;

    Interpret_As_IBM_01145(const Interpret_As_IBM_01145&) = delete;
    Interpret_As_IBM_01145& operator=(const Interpret_As_IBM_01145&) = delete;

    QString decodeIBM01145(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
