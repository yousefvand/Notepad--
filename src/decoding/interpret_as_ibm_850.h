#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_850 {
public:
    static Interpret_As_IBM_850& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_850();
    ~Interpret_As_IBM_850() = default;

    Interpret_As_IBM_850(const Interpret_As_IBM_850&) = delete;
    Interpret_As_IBM_850& operator=(const Interpret_As_IBM_850&) = delete;

    QString decodeIBM850(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
