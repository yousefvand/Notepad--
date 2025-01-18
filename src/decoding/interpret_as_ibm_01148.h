#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_01148 {
public:
    static Interpret_As_IBM_01148& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_01148();
    ~Interpret_As_IBM_01148() = default;

    Interpret_As_IBM_01148(const Interpret_As_IBM_01148&) = delete;
    Interpret_As_IBM_01148& operator=(const Interpret_As_IBM_01148&) = delete;

    QString decodeIBM01148(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
