#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_273 {
public:
    static Interpret_As_IBM_273& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_273();
    ~Interpret_As_IBM_273() = default;

    Interpret_As_IBM_273(const Interpret_As_IBM_273&) = delete;
    Interpret_As_IBM_273& operator=(const Interpret_As_IBM_273&) = delete;

    QString decodeIBM273(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
