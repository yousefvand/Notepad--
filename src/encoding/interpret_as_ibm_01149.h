#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_01149 {
public:
    static Interpret_As_IBM_01149& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_01149();
    ~Interpret_As_IBM_01149() = default;

    Interpret_As_IBM_01149(const Interpret_As_IBM_01149&) = delete;
    Interpret_As_IBM_01149& operator=(const Interpret_As_IBM_01149&) = delete;

    QString decodeIBM01149(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
