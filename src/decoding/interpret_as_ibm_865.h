#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_865 {
public:
    static Interpret_As_IBM_865& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_865();
    ~Interpret_As_IBM_865() = default;

    Interpret_As_IBM_865(const Interpret_As_IBM_865&) = delete;
    Interpret_As_IBM_865& operator=(const Interpret_As_IBM_865&) = delete;

    QString decodeIBM865(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
