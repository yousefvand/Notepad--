#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_01146 {
public:
    static Interpret_As_IBM_01146& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_01146();
    ~Interpret_As_IBM_01146() = default;

    Interpret_As_IBM_01146(const Interpret_As_IBM_01146&) = delete;
    Interpret_As_IBM_01146& operator=(const Interpret_As_IBM_01146&) = delete;

    QString decodeIBM01146(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
