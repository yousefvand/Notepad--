#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_869 {
public:
    static Interpret_As_IBM_869& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_869();
    ~Interpret_As_IBM_869() = default;

    Interpret_As_IBM_869(const Interpret_As_IBM_869&) = delete;
    Interpret_As_IBM_869& operator=(const Interpret_As_IBM_869&) = delete;

    QString decodeIBM869(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
