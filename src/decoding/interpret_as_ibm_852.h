#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_852 {
public:
    static Interpret_As_IBM_852& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_852();
    ~Interpret_As_IBM_852() = default;

    Interpret_As_IBM_852(const Interpret_As_IBM_852&) = delete;
    Interpret_As_IBM_852& operator=(const Interpret_As_IBM_852&) = delete;

    QString decodeIBM852(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
