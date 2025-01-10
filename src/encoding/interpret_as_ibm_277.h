#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_277 {
public:
    static Interpret_As_IBM_277& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_277();
    ~Interpret_As_IBM_277() = default;

    Interpret_As_IBM_277(const Interpret_As_IBM_277&) = delete;
    Interpret_As_IBM_277& operator=(const Interpret_As_IBM_277&) = delete;

    QString decodeIBM277(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
