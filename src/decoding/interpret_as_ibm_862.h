#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_862 {
public:
    static Interpret_As_IBM_862& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_862();
    ~Interpret_As_IBM_862() = default;

    Interpret_As_IBM_862(const Interpret_As_IBM_862&) = delete;
    Interpret_As_IBM_862& operator=(const Interpret_As_IBM_862&) = delete;

    QString decodeIBM862(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
