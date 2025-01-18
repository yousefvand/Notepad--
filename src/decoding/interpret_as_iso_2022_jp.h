#pragma once
#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_ISO_2022_JP {
public:
    static Interpret_As_ISO_2022_JP& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_ISO_2022_JP();
    ~Interpret_As_ISO_2022_JP() = default;

    Interpret_As_ISO_2022_JP(const Interpret_As_ISO_2022_JP&) = delete;
    Interpret_As_ISO_2022_JP& operator=(const Interpret_As_ISO_2022_JP&) = delete;

    QString decodeISO2022JP(const QByteArray& rawData);
    static const std::unordered_map<QByteArray, QChar> jisTable;
};
