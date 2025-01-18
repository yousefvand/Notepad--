#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_870 {
public:
    static Interpret_As_IBM_870& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_870();
    ~Interpret_As_IBM_870() = default;

    Interpret_As_IBM_870(const Interpret_As_IBM_870&) = delete;
    Interpret_As_IBM_870& operator=(const Interpret_As_IBM_870&) = delete;

    QString decodeIBM870(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
