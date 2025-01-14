#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_01143 {
public:
    static Interpret_As_IBM_01143& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_01143();
    ~Interpret_As_IBM_01143() = default;

    Interpret_As_IBM_01143(const Interpret_As_IBM_01143&) = delete;
    Interpret_As_IBM_01143& operator=(const Interpret_As_IBM_01143&) = delete;

    QString decodeIBM01143(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
