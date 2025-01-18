#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_278 {
public:
    static Interpret_As_IBM_278& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_278();
    ~Interpret_As_IBM_278() = default;

    Interpret_As_IBM_278(const Interpret_As_IBM_278&) = delete;
    Interpret_As_IBM_278& operator=(const Interpret_As_IBM_278&) = delete;

    QString decodeIBM278(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
