#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_01141 {
public:
    static Interpret_As_IBM_01141& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_01141();
    ~Interpret_As_IBM_01141() = default;

    Interpret_As_IBM_01141(const Interpret_As_IBM_01141&) = delete;
    Interpret_As_IBM_01141& operator=(const Interpret_As_IBM_01141&) = delete;

    QString decodeIBM01141(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
