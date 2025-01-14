#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_01142 {
public:
    static Interpret_As_IBM_01142& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_01142();
    ~Interpret_As_IBM_01142() = default;

    Interpret_As_IBM_01142(const Interpret_As_IBM_01142&) = delete;
    Interpret_As_IBM_01142& operator=(const Interpret_As_IBM_01142&) = delete;

    QString decodeIBM01142(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
