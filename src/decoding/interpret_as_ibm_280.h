#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_280 {
public:
    static Interpret_As_IBM_280& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_280();
    ~Interpret_As_IBM_280() = default;

    Interpret_As_IBM_280(const Interpret_As_IBM_280&) = delete;
    Interpret_As_IBM_280& operator=(const Interpret_As_IBM_280&) = delete;

    QString decodeIBM280(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
