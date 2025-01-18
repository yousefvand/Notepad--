#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_1047 {
public:
    static Interpret_As_IBM_1047& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_1047();
    ~Interpret_As_IBM_1047() = default;

    Interpret_As_IBM_1047(const Interpret_As_IBM_1047&) = delete;
    Interpret_As_IBM_1047& operator=(const Interpret_As_IBM_1047&) = delete;

    QString decodeIBM1047(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
