#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_500 {
public:
    static Interpret_As_IBM_500& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_500();
    ~Interpret_As_IBM_500() = default;

    Interpret_As_IBM_500(const Interpret_As_IBM_500&) = delete;
    Interpret_As_IBM_500& operator=(const Interpret_As_IBM_500&) = delete;

    QString decodeIBM500(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
