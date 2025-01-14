#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_285 {
public:
    static Interpret_As_IBM_285& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_285();
    ~Interpret_As_IBM_285() = default;

    Interpret_As_IBM_285(const Interpret_As_IBM_285&) = delete;
    Interpret_As_IBM_285& operator=(const Interpret_As_IBM_285&) = delete;

    QString decodeIBM285(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
