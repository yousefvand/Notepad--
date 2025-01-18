#pragma once
#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_871 {
public:
    static Interpret_As_IBM_871& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_871();
    ~Interpret_As_IBM_871() = default;

    Interpret_As_IBM_871(const Interpret_As_IBM_871&) = delete;
    Interpret_As_IBM_871& operator=(const Interpret_As_IBM_871&) = delete;

    QString decodeIBM871(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
