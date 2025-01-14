#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_864 {
public:
    static Interpret_As_IBM_864& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_864();
    ~Interpret_As_IBM_864() = default;

    Interpret_As_IBM_864(const Interpret_As_IBM_864&) = delete;
    Interpret_As_IBM_864& operator=(const Interpret_As_IBM_864&) = delete;

    QString decodeIBM864(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
