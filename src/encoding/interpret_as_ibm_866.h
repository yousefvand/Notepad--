#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_866 {
public:
    static Interpret_As_IBM_866& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_866();
    ~Interpret_As_IBM_866() = default;

    Interpret_As_IBM_866(const Interpret_As_IBM_866&) = delete;
    Interpret_As_IBM_866& operator=(const Interpret_As_IBM_866&) = delete;

    QString decodeIBM866(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
