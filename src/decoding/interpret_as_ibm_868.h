#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_868 {
public:
    static Interpret_As_IBM_868& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_868();
    ~Interpret_As_IBM_868() = default;

    Interpret_As_IBM_868(const Interpret_As_IBM_868&) = delete;
    Interpret_As_IBM_868& operator=(const Interpret_As_IBM_868&) = delete;

    QString decodeIBM868(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
