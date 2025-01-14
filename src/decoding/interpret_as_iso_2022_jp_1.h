#pragma once
#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_ISO_2022_JP_1 {
public:
    static Interpret_As_ISO_2022_JP_1& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_ISO_2022_JP_1();
    ~Interpret_As_ISO_2022_JP_1() = default;

    Interpret_As_ISO_2022_JP_1(const Interpret_As_ISO_2022_JP_1&) = delete;
    Interpret_As_ISO_2022_JP_1& operator=(const Interpret_As_ISO_2022_JP_1&) = delete;

    QString decodeISO2022JP1(const QByteArray& rawData);
    static const std::unordered_map<QByteArray, QChar> jisTable;
};
