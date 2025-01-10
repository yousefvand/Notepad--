#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_290 {
public:
    static Interpret_As_IBM_290& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_290();
    ~Interpret_As_IBM_290() = default;

    Interpret_As_IBM_290(const Interpret_As_IBM_290&) = delete;
    Interpret_As_IBM_290& operator=(const Interpret_As_IBM_290&) = delete;

    QString decodeIBM290(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
