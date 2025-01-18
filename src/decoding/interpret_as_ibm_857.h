#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_857 {
public:
    static Interpret_As_IBM_857& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_857();
    ~Interpret_As_IBM_857() = default;

    Interpret_As_IBM_857(const Interpret_As_IBM_857&) = delete;
    Interpret_As_IBM_857& operator=(const Interpret_As_IBM_857&) = delete;

    QString decodeIBM857(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
