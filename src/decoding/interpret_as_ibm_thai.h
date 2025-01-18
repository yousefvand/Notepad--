#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_Thai {
public:
    static Interpret_As_IBM_Thai& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_Thai();
    ~Interpret_As_IBM_Thai() = default;

    Interpret_As_IBM_Thai(const Interpret_As_IBM_Thai&) = delete;
    Interpret_As_IBM_Thai& operator=(const Interpret_As_IBM_Thai&) = delete;

    QString decodeIBMThai(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> codePageTable;
};
