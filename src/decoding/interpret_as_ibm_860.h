#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_860 {
public:
    static Interpret_As_IBM_860& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_860();
    ~Interpret_As_IBM_860() = default;

    Interpret_As_IBM_860(const Interpret_As_IBM_860&) = delete;
    Interpret_As_IBM_860& operator=(const Interpret_As_IBM_860&) = delete;

    QString decodeIBM860(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
