#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_284 {
public:
    static Interpret_As_IBM_284& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_284();
    ~Interpret_As_IBM_284() = default;

    Interpret_As_IBM_284(const Interpret_As_IBM_284&) = delete;
    Interpret_As_IBM_284& operator=(const Interpret_As_IBM_284&) = delete;

    QString decodeIBM284(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
