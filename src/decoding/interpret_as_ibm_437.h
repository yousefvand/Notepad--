#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_437 {
public:
    static Interpret_As_IBM_437& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_437();
    ~Interpret_As_IBM_437() = default;

    Interpret_As_IBM_437(const Interpret_As_IBM_437&) = delete;
    Interpret_As_IBM_437& operator=(const Interpret_As_IBM_437&) = delete;

    QString decodeIBM437(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> asciiTable;
};
