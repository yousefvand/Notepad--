#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_855 {
public:
    static Interpret_As_IBM_855& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_855();
    ~Interpret_As_IBM_855() = default;

    Interpret_As_IBM_855(const Interpret_As_IBM_855&) = delete;
    Interpret_As_IBM_855& operator=(const Interpret_As_IBM_855&) = delete;

    QString decodeIBM855(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
