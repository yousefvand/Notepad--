#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_1026 {
public:
    static Interpret_As_IBM_1026& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_1026();
    ~Interpret_As_IBM_1026() = default;

    Interpret_As_IBM_1026(const Interpret_As_IBM_1026&) = delete;
    Interpret_As_IBM_1026& operator=(const Interpret_As_IBM_1026&) = delete;

    QString decodeIBM1026(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
