#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_424 {
public:
    static Interpret_As_IBM_424& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_424();
    ~Interpret_As_IBM_424() = default;

    Interpret_As_IBM_424(const Interpret_As_IBM_424&) = delete;
    Interpret_As_IBM_424& operator=(const Interpret_As_IBM_424&) = delete;

    QString decodeIBM424(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
