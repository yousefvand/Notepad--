#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_861 {
public:
    static Interpret_As_IBM_861& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_861();
    ~Interpret_As_IBM_861() = default;

    Interpret_As_IBM_861(const Interpret_As_IBM_861&) = delete;
    Interpret_As_IBM_861& operator=(const Interpret_As_IBM_861&) = delete;

    QString decodeIBM861(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
