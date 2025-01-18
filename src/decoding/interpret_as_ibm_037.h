#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_037 {
public:
    static Interpret_As_IBM_037& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_037();
    ~Interpret_As_IBM_037() = default;

    Interpret_As_IBM_037(const Interpret_As_IBM_037&) = delete;
    Interpret_As_IBM_037& operator=(const Interpret_As_IBM_037&) = delete;

    QString decodeIBM037(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
