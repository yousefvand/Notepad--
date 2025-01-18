#pragma once
#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_ISO_2022_JP_2 {
public:
    static Interpret_As_ISO_2022_JP_2& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_ISO_2022_JP_2();
    ~Interpret_As_ISO_2022_JP_2() = default;

    Interpret_As_ISO_2022_JP_2(const Interpret_As_ISO_2022_JP_2&) = delete;
    Interpret_As_ISO_2022_JP_2& operator=(const Interpret_As_ISO_2022_JP_2&) = delete;

    QString decodeISO2022JP2(const QByteArray& rawData);
    static const std::unordered_map<QByteArray, QChar> jisTable;
};
