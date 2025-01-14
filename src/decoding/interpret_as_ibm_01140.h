#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_01140 {
public:
    static Interpret_As_IBM_01140& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_01140();
    ~Interpret_As_IBM_01140() = default;

    Interpret_As_IBM_01140(const Interpret_As_IBM_01140&) = delete;
    Interpret_As_IBM_01140& operator=(const Interpret_As_IBM_01140&) = delete;

    QString decodeIBM01140(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
