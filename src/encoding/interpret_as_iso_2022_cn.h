#pragma once
#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_ISO_2022_CN {
public:
    static Interpret_As_ISO_2022_CN& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_ISO_2022_CN();
    ~Interpret_As_ISO_2022_CN() = default;

    Interpret_As_ISO_2022_CN(const Interpret_As_ISO_2022_CN&) = delete;
    Interpret_As_ISO_2022_CN& operator=(const Interpret_As_ISO_2022_CN&) = delete;

    QString decodeISO2022CN(const QByteArray& rawData);
    static const std::unordered_map<QByteArray, QChar> gbTable;
};
