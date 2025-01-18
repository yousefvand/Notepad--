#pragma once
#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_ISO_2022_CN_EXT {
public:
    static Interpret_As_ISO_2022_CN_EXT& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_ISO_2022_CN_EXT();
    ~Interpret_As_ISO_2022_CN_EXT() = default;

    Interpret_As_ISO_2022_CN_EXT(const Interpret_As_ISO_2022_CN_EXT&) = delete;
    Interpret_As_ISO_2022_CN_EXT& operator=(const Interpret_As_ISO_2022_CN_EXT&) = delete;

    QString decodeISO2022CNEXT(const QByteArray& rawData);
    static const std::unordered_map<QByteArray, QChar> gbTable;
};
