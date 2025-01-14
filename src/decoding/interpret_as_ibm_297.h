#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_297 {
public:
    static Interpret_As_IBM_297& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_297();
    ~Interpret_As_IBM_297() = default;

    Interpret_As_IBM_297(const Interpret_As_IBM_297&) = delete;
    Interpret_As_IBM_297& operator=(const Interpret_As_IBM_297&) = delete;

    QString decodeIBM297(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
