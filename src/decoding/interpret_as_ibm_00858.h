#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_00858 {
public:
    static Interpret_As_IBM_00858& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_00858();
    ~Interpret_As_IBM_00858() = default;

    Interpret_As_IBM_00858(const Interpret_As_IBM_00858&) = delete;
    Interpret_As_IBM_00858& operator=(const Interpret_As_IBM_00858&) = delete;

    QString decodeIBM00858(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> codePageTable;
};
